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
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>
#include <ls/CompositeContext.h>
#include "yoga-ext.h"

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
                    self->image->GetWidthF(),
                    self->image->GetHeightF(),
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
        this->QueueAfterLayout();

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
        this->QueueAfterLayout();
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
                    this->QueueAfterLayout();
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

void ImageSceneNode::OnPropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::tintColor:
            // TODO: add am immediate mode flag
            if (this->layer) {
                this->QueuePaint();
            } else {
                this->QueueComposite();
            }
            break;
        case StyleProperty::borderColor:
            this->QueuePaint();
            break;
        case StyleProperty::objectFit:
        case StyleProperty::objectPositionX:
        case StyleProperty::objectPositionY:
            this->QueueAfterLayout();
            break;
        default:
            if (IsYogaLayoutProperty(property)) {
                this->QueueAfterLayout();
            }
            break;
    }
}

void ImageSceneNode::BeforeLayout() {
}

void ImageSceneNode::AfterLayout() {
    if (!this->image) {
        return;
    }

    YGNodeSetHasNewLayout(this->ygNode, false);

    const auto boxStyle{ this->GetStyleOrEmpty() };

    this->destRect = ComputeObjectFitRect(
        boxStyle->objectFit,
        boxStyle->objectPositionX,
        boxStyle->objectPositionY,
        YGNodeLayoutGetInnerRect(this->ygNode),
        this->image.Get(),
        this->scene);

    this->QueuePaint();
}

void ImageSceneNode::Paint(Renderer* renderer) {
    if (!this->image) {
        return;
    }

    this->image->Sync(renderer);

    const auto boxStyle{ this->GetStyleOrEmpty() };
    const auto& borderColor{ boxStyle->borderColor };
    const auto hasCapInsets{ this->image->HasCapInsets() };

    if (!hasCapInsets && borderColor.empty()) {
        this->QueueComposite();
        return;
    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(rect.width), static_cast<int32_t>(rect.height))) {
        return;
    }

    renderer->FillRenderTarget(0);

    if (this->image->HasTexture()) {
        const auto tintColor{ boxStyle->tintColor.ValueOr(RGB(255, 255, 255)) };

        if (hasCapInsets) {
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

    if (!borderColor.empty()) {
        renderer->DrawBorder(rect, YGNodeLayoutGetBorderRect(this->ygNode), borderColor.value);
    }

    renderer->SetRenderTarget(nullptr);
    this->QueueComposite();
}

void ImageSceneNode::Composite(CompositeContext* context) {
    if (!this->image || !this->image->HasTexture() || !this->layer) {
        return;
    }

    const auto boxStyle{ this->GetStyleOrEmpty() };
    const auto& transform{ context->CurrentMatrix() };
    const auto tx{ transform.GetTranslateX() };
    const auto ty{ transform.GetTranslateY() };

    if (this->layer) {
        context->renderer->DrawImage(
            this->layer,
            YGNodeLayoutGetRect(this->ygNode, tx, ty),
            RGB(255, 255, 255));
    } else {
        const auto tintColor{ boxStyle->tintColor.ValueOr(RGB(255, 255, 255)) };
        auto dest{ this->destRect };

        dest.x += tx;
        dest.y += ty;

        // TODO: must do a manual clip of dest rect!
        context->PushClipRect(YGNodeLayoutGetRect(this->ygNode, tx, ty));
        context->renderer->DrawImage(this->image->GetTexture(), dest, tintColor);
        context->PopClipRect();
    }
}

void ImageSceneNode::DestroyRecursive() {
    this->image = nullptr;
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
