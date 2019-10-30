/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageSceneNode.h"
#include "ImageStore.h"
#include "Style.h"
#include "Scene.h"
#include "StyleUtils.h"
#include "CompositeContext.h"
#include <ls/Math.h>
#include <ls/Renderer.h>
#include "yoga-ext.h"
#include <ls/Log.h>
#include <ls/PixelConversion.h>

using Napi::Array;
using Napi::Call;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

ImageSceneNode::ImageSceneNode(const CallbackInfo& info) : ObjectWrap<ImageSceneNode>(info), SceneNode(info) {
    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            const auto self { static_cast<ImageSceneNode *>(YGNodeGetContext(nodeRef)) };

            if (self && self->image && self->image->IsReady()) {
                return {
                    static_cast<float>(self->image->GetWidth()),
                    static_cast<float>(self->image->GetHeight()),
                };
            }

            return { 0.f, 0.f };
    });
}

Function ImageSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(
            env,
            "ImageSceneNode",
            SceneNode::Extend<ImageSceneNode>(env, {
                InstanceAccessor("src", &ImageSceneNode::GetSource, &ImageSceneNode::SetSource),
                InstanceAccessor("onLoad", &ImageSceneNode::GetOnLoadCallback, &ImageSceneNode::SetOnLoadCallback),
                InstanceAccessor("onError", &ImageSceneNode::GetOnErrorCallback, &ImageSceneNode::SetOnErrorCallback),
            }));

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value ImageSceneNode::GetSource(const CallbackInfo& info) {
    if (this->uri.GetId().empty()) {
        return info.Env().Null();
    } else {
        return this->uri.ToObject(info.Env());
    }
}

void ImageSceneNode::SetSource(const CallbackInfo& info, const Napi::Value& value) {
    const std::string src{ value.IsString() ? value.As<String>().Utf8Value() : "" };

    if (src.empty()) {
        this->image = nullptr;
        YGNodeMarkDirty(this->ygNode);

        return;
    }

    const ImageUri newUri(src);

    if (newUri == this->uri) {
        return;
    }

    this->uri = newUri;

    // TODO: LoadImage exception?
    this->image = this->scene->GetImageStore()->GetOrLoadImage(this->uri);

    if (!this->image || this->image->IsReady() || this->image->HasError()) {
        YGNodeMarkDirty(this->ygNode);
        DoCallbacks();
    } else {
        this->image.Listen([this, ptr = this->image.Get()]() {
            if (this->image != ptr) {
                return;
            }

            switch (this->image->GetState()) {
                case ResourceStateReady:
                case ResourceStateError:
                    YGNodeMarkDirty(this->ygNode);
                    this->image.Unlisten();
                    this->DoCallbacks();
                    break;
                default:
                    break;
            }
        });
    }
}

Value ImageSceneNode::GetOnLoadCallback(const CallbackInfo& info) {
    return this->onLoadCallback.Value();
}

void ImageSceneNode::SetOnLoadCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (info[0].IsFunction()) {
        this->onLoadCallback.Reset(info[0].As<Function>(), 1);
    } else {
        this->onLoadCallback.Reset();
    }
}

Value ImageSceneNode::GetOnErrorCallback(const CallbackInfo& info) {
    return this->onErrorCallback.Value();
}

void ImageSceneNode::SetOnErrorCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (info[0].IsFunction()) {
        this->onErrorCallback.Reset(info[0].As<Function>(), 1);
    } else {
        this->onErrorCallback.Reset();
    }
}

void ImageSceneNode::ComputeStyle() {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    if (!this->image) {
        return;
    }

    auto bounds{ YGNodeLayoutGetInnerRect(this->ygNode) };
    auto fit{ CalculateObjectFitDimensions(
        boxStyle->objectFit(),
        this->image.Get(),
        bounds.width,
        bounds.height)
    };
    auto offsetX{ CalculateObjectPosition(
        boxStyle->objectPositionX(),
        true,
        bounds.width,
        fit.width,
        0.5f,
        this->scene)
    };
    auto offsetY{ CalculateObjectPosition(
        boxStyle->objectPositionY(),
        false,
        bounds.height,
        fit.height,
        0.5f,
        this->scene)
    };

//    this->srcRect = {
//        0.f,
//        0.f,
//        static_cast<float>(this->image->GetWidth()),
//        static_cast<float>(this->image->GetHeight()),
//    };

    this->destRect = {
        SnapToPixelGrid(bounds.x + offsetX),
        SnapToPixelGrid(bounds.y + offsetY),
        SnapToPixelGrid(fit.width),
        SnapToPixelGrid(fit.height),
    };

    // TODO: clip dest against bounds
}

void ImageSceneNode::Paint(Renderer* renderer) {
    if (!this->image) {
        return;
    }

    this->image->Sync(renderer);

    const auto boxStyle{ this->GetStyleOrEmpty() };
    auto rect{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (this->image->HasCapInsets() || boxStyle->HasBorder()) {
        if (!this->layer) {
            this->layer = renderer->CreateRenderTarget(
                static_cast<int32_t>(rect.width), static_cast<int32_t>(rect.height));
        }

        renderer->SetRenderTarget(this->layer);
        renderer->FillRenderTarget(0);

        if (this->image->GetTexture()) {
            const auto tintColor{ boxStyle->tintColor() ? boxStyle->tintColor()->Get() : RGB(255, 255, 255) };

            if (this->image->HasCapInsets()) {
                renderer->DrawImage(
                    this->image->GetTexture(),
                    this->destRect,
                    this->image->GetCapInsets(),
                    tintColor);
            } else {
                renderer->DrawImage(
                    this->image->GetTexture(),
                    this->destRect,
                    tintColor);
            }
        }

        if (boxStyle->HasBorder() && boxStyle->borderColor()) {
            renderer->DrawBorder(
                rect,
                YGNodeLayoutGetBorderRect(this->ygNode),
                boxStyle->borderColor()->Get());
        }
    }
}

void ImageSceneNode::Composite(CompositeContext* context, Renderer* renderer) {
    if (!this->image || !this->image->GetTexture() || !this->layer) {
        return;
    }

    const auto boxStyle{ this->GetStyleOrEmpty() };
    const auto tintColor{ boxStyle->tintColor() ? boxStyle->tintColor()->Get() : RGB(255, 255, 255) };
    auto transform{ context->CurrentMatrix() };
    auto dest{ YGNodeLayoutGetRect(this->ygNode) };

    dest.x += (transform.GetTranslateX());
    dest.y += (transform.GetTranslateY());

    renderer->DrawImage(this->layer ? this->layer : this->image->GetTexture(), dest, tintColor);
}

void ImageSceneNode::DestroyRecursive() {
    this->image = nullptr;
    this->layer = nullptr;
    this->onLoadCallback.Reset();
    this->onErrorCallback.Reset();

    SceneNode::DestroyRecursive();
}

void ImageSceneNode::DoCallbacks() {
    switch (this->image ? this->image->GetState() : ResourceStateInit) {
        case ResourceStateReady:
            Call(this->onLoadCallback);
            break;
        case ResourceStateError:
            Call(this->onErrorCallback);
            break;
        default:
            break;
    }
}

void ImageSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on img nodes");
}

} // namespace ls
