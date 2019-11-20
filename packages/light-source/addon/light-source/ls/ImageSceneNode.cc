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
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

ImageSceneNode::ImageSceneNode(const CallbackInfo& info) : SafeObjectWrap<ImageSceneNode>(info), SceneNode(info) {
}

void ImageSceneNode::Constructor(const Napi::CallbackInfo& info) {
    SceneNode::BaseConstructor(info);

    if (this->scene == nullptr) {
        return;
    }

    YGNodeSetContext(this->ygNode, this);

    YGNodeSetMeasureFunc(
    this->ygNode,
    [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
        const auto self { static_cast<ImageSceneNode*>(YGNodeGetContext(nodeRef)) };

        if (self && self->image && self->image->IsReady()) {
            return { self->image->GetWidthF(), self->image->GetHeightF() };
        } else {
            return { 0.f, 0.f };
        }
    });
}

Function ImageSceneNode::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(
            env,
            "ImageSceneNode", true,
            SceneNode::Extend<ImageSceneNode>(env, {
                InstanceAccessor("src", &ImageSceneNode::GetSource, &ImageSceneNode::SetSource),
                InstanceAccessor("onLoad", &ImageSceneNode::GetOnLoadCallback, &ImageSceneNode::SetOnLoadCallback),
                InstanceAccessor("onError", &ImageSceneNode::GetOnErrorCallback, &ImageSceneNode::SetOnErrorCallback),
            }));
    }

    return constructor.Value();
}

Value ImageSceneNode::GetSource(const CallbackInfo& info) {
    auto env{ info.Env() };
    Napi::Value result;
    EscapableHandleScope scope(env);

    if (this->uri.IsEmpty()) {
        result = env.Null();
    } else {
        result = ImageUri::Box(env, this->uri);
    }

    return scope.Escape(result);
}

void ImageSceneNode::SetSource(const CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto imageUri{ ImageUri::Unbox(value) };

    if (imageUri.IsEmpty()) {
        if (this->image) {
            this->image = nullptr;
            this->uri = {};
            this->MarkDirty();
        }
        return;
    } else if (this->uri == imageUri) {
        return;
    }

    try {
        this->image = this->scene->GetImageStore()->GetOrLoadImage(imageUri.GetId());
    } catch (const std::exception& e) {
        LOG_WARN("%s", e);
        this->image = nullptr;
    }

    if (!this->image) {
        this->uri = {};
        this->MarkDirty();
        Call(this->onErrorCallback);
        return;
    }

    std::exchange(this->uri, imageUri);

    if (this->image->IsReady() || this->image->HasError()) {
        this->MarkDirty();
        DoCallbacks();
    } else {
        this->image.Listen([this, ptr = this->image.Get()]() {
            if (this->image != ptr) {
                return;
            }

            switch (this->image->GetState()) {
                case ResourceStateReady:
                case ResourceStateError:
                    this->image.Unlisten();
                    this->MarkDirty();
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
        case StyleProperty::transform:
        case StyleProperty::transformOriginX:
        case StyleProperty::transformOriginY:
        case StyleProperty::opacity:
            this->QueueComposite();
            break;
        case StyleProperty::overflow:
            break;
        default:
            SceneNode::OnPropertyChanged(property);
            break;
    }
}

void ImageSceneNode::BeforeLayout() {
}

void ImageSceneNode::AfterLayout() {
    if (!this->image) {
        return;
    }

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

    this->QueueComposite();

    if (!hasCapInsets && borderColor.empty()) {
        this->layer = nullptr;
        return;
    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(rect.width), static_cast<int32_t>(rect.height))) {
        return;
    }

    renderer->FillRenderTarget(ColorTransparent);

    if (this->image->HasTexture()) {
        if (hasCapInsets) {
            renderer->DrawImage(
                this->image->GetTexture(),
                this->destRect,
                this->image->GetCapInsets(),
                boxStyle->tintColor.ValueOr(ColorWhite));
        } else {
            renderer->DrawImage(
                this->image->GetTexture(),
                this->destRect,
                boxStyle->tintColor.ValueOr(ColorWhite));
        }
    }

    if (!borderColor.empty()) {
        renderer->DrawBorder(rect, YGNodeLayoutGetBorderRect(this->ygNode), borderColor.value);
    }

    renderer->SetRenderTarget(nullptr);
}

void ImageSceneNode::Composite(CompositeContext* context) {
    if (this->layer) {
        const auto boxStyle{ this->GetStyleOrEmpty() };
        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
        const auto transform{
            context->CurrentMatrix()
                * Matrix::Translate(rect.x, rect.y)
                * boxStyle->transform.ToMatrix(rect.width, rect.height)
        };

        context->renderer->DrawImage(
            this->layer,
            { 0, 0, rect.width, rect.height },
            transform,
            ARGB(context->CurrentOpacityAlpha(), 255, 255, 255));
    } else if (this->image && this->image->HasTexture()) {
        const auto boxStyle{ this->GetStyleOrEmpty() };
        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
        const auto transform{
            context->CurrentMatrix()
                * Matrix::Translate(rect.x, rect.y)
                * boxStyle->transform.ToMatrix(rect.width, rect.height)
        };

        context->renderer->DrawImage(
            this->image->GetTexture(),
            this->destRect,
            transform,
            MixAlpha(boxStyle->tintColor.ValueOr(ColorWhite), context->CurrentOpacity()));
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
