/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageSceneNode.h"
#include "Style.h"
#include "Scene.h"
#include "Stage.h"
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
    SceneNode::BaseConstructor(info, SceneNodeTypeImage);

    YGNodeSetContext(this->ygNode, this);

    YGNodeSetMeasureFunc(
    this->ygNode,
    [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
//        const auto self { static_cast<ImageSceneNode*>(YGNodeGetContext(nodeRef)) };

        // TODO: return w/h from image data
//        if (self && self->image && self->image->IsReady()) {
//            return { self->image->GetWidthF(), self->image->GetHeightF() };
//        } else {
//            return { 0.f, 0.f };
//        }
        return { 0.f, 0.f };
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
    return String::New(info.Env(), this->src);
}

void ImageSceneNode::SetSource(const CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    std::string newSrc{};

    switch (value.Type()) {
        case napi_string:
            newSrc = value.As<String>();
            break;
        case napi_null:
        case napi_undefined:
            break;
        default:
            throw Error::New(env, "src must be a string");
    }

    if (newSrc == this->src) {
        return;
    }

    if (newSrc.empty()) {
        if (!this->src.empty()) {
            this->src.clear();
            this->ClearResource();
            this->MarkDirty();
        }
        return;
    }

    if (this->image) {
        this->MarkDirty();
    }

    this->ClearResource();
    this->src = newSrc;
    this->image = this->GetStage()->GetResources()->AcquireImageData(this->src);

    auto listener{ [this](Res::Owner owner, Res* res) {
        constexpr auto LAMBDA_FUNCTION = "ImageResourceListener";

      if (this != owner || this->image != res) {
          LOG_WARN_LAMBDA("Invalid owner or resource: %s", this->src);
          return;
      }

      Napi::HandleScope scope(this->Env());

      switch (res->GetState()) {
          case Res::Ready:
              if (!this->onLoadCallback.IsEmpty()) {
                  try {
                      this->onLoadCallback.Call({this->Value(), res->GetSummary(this->Env())});
                  } catch (std::exception& e) {
                      LOG_WARN_LAMBDA("onLoad unhandled exception: %s", e);
                  }
              }
              break;
          case Res::Error:
              if (!this->onErrorCallback.IsEmpty()) {
                  try {
                      this->onErrorCallback.Call({this->Value(), res->GetErrorMessage(this->Env())});
                  } catch (std::exception& e) {
                      LOG_WARN_LAMBDA("onError unhandled exception: %s", e);
                  }
              }
              break;
          default:
              break;
      }

      this->MarkDirty();
      res->RemoveListener(owner);
    }};

    switch (this->image->GetState()) {
        case Res::State::Init:
            this->image->AddListener(this, listener);
            this->image->Load(env);
            break;
        case Res::State::Loading:
            this->image->AddListener(this, listener);
            break;
        case Res::State::Ready:
        case Res::State::Error:
            listener(this, this->image);
            break;
    }
}

Value ImageSceneNode::GetOnLoadCallback(const CallbackInfo& info) {
    return this->onLoadCallback.Value();
}

void ImageSceneNode::SetOnLoadCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (!Napi::AssignFunctionReference(this->onLoadCallback, value)) {
        throw Error::New(info.Env(), "Invalid assignment of onLoad.");
    }
}

Value ImageSceneNode::GetOnErrorCallback(const CallbackInfo& info) {
    return this->onErrorCallback.Value();
}

void ImageSceneNode::SetOnErrorCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (!Napi::AssignFunctionReference(this->onErrorCallback, value)) {
        throw Error::New(info.Env(), "Invalid assignment of onError.");
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
//    if (!this->image || !this->image->IsReady()) {
//        return;
//    }
//
//    const auto boxStyle{ this->GetStyleOrEmpty() };
//    const auto innerRect{ YGNodeLayoutGetInnerRect(this->ygNode) };
//    const auto imageRect = ComputeObjectFitRect(
//        boxStyle->objectFit,
//        boxStyle->objectPositionX,
//        boxStyle->objectPositionY,
//        innerRect,
//        this->image.Get(),
//        this->scene);
//
//    if (!image->HasCapInsets() && boxStyle->borderColor.empty()) {
//        const auto imageWidth{ this->image->GetWidthF() };
//        const auto imageHeight{ this->image->GetHeightF() };
//
//        if (imageWidth == 0 || imageHeight == 0) {
//            this->destRect = {};
//            this->srcRect = {};
//        } else {
//            const auto scaleX{ imageRect.width / imageWidth };
//            const auto scaleY{ imageRect.height / imageHeight };
//
//            this->destRect = Intersect(innerRect, imageRect);
//            this->srcRect = {
//                std::max(innerRect.x - imageRect.x, 0.f) * scaleX,
//                std::max(innerRect.y - imageRect.y, 0.f) * scaleY,
//                this->destRect.width * scaleX,
//                this->destRect.height * scaleY };
//        }
//    } else {
//        this->destRect = imageRect;
//    }
//
//    this->QueuePaint();
}

void ImageSceneNode::Paint(PaintContext* paint) {
//    if (!this->image) {
//        return;
//    }
//
//    auto renderer{ paint->renderer };
//    const auto boxStyle{ this->GetStyleOrEmpty() };
//    const auto& borderColor{ boxStyle->borderColor };
//    const auto hasCapInsets{ this->image->HasCapInsets() };
//
//    this->QueueComposite();
//    this->image->Sync(renderer);
//
//    if (!hasCapInsets && borderColor.empty()) {
//        this->layer = nullptr;
//        return;
//    }
//
//    const auto rect{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
//
//    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(rect.width), static_cast<int32_t>(rect.height))) {
//        return;
//    }
//
//    renderer->FillRenderTarget(ColorTransparent);
//
//    if (this->image->HasTexture()) {
//        if (hasCapInsets) {
//            renderer->DrawImage(
//                this->image->GetTexture(),
//                this->destRect,
//                this->image->GetCapInsets(),
//                boxStyle->tintColor.ValueOr(ColorWhite));
//        } else {
//            renderer->DrawImage(
//                this->image->GetTexture(),
//                this->destRect,
//                boxStyle->tintColor.ValueOr(ColorWhite));
//        }
//    }
//
//    if (!borderColor.empty()) {
//        renderer->DrawBorder(rect, YGNodeLayoutGetBorderRect(this->ygNode), borderColor.value);
//    }
//
//    renderer->SetRenderTarget(nullptr);
}

void ImageSceneNode::Composite(CompositeContext* composite) {
//    if (this->layer) {
//        const auto boxStyle{ this->GetStyleOrEmpty() };
//        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
//        const auto transform{
//            ComputeTransform(
//                composite->CurrentMatrix(),
//                boxStyle->transform,
//                boxStyle->transformOriginX,
//                boxStyle->transformOriginY,
//                rect,
//                this->scene)
//        };
//
//        composite->renderer->DrawImage(
//            this->layer,
//            rect,
//            transform,
//            ARGB(composite->CurrentOpacityAlpha(), 255, 255, 255));
//    } else if (this->image && this->image->HasTexture()) {
//        const auto boxStyle{ this->GetStyleOrEmpty() };
//        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
//        const auto transform{
//            ComputeTransform(
//                composite->CurrentMatrix(),
//                boxStyle->transform,
//                boxStyle->transformOriginX,
//                boxStyle->transformOriginY,
//                rect,
//                this->scene)
//        };
//
//        composite->renderer->DrawImage(
//            this->image->GetTexture(),
//            this->srcRect,
//            Translate(this->destRect, rect.x, rect.y),
//            { rect.x, rect.y },
//            transform,
//            MixAlpha(boxStyle->tintColor.ValueOr(ColorWhite), composite->CurrentOpacity()));
//    }
}

void ImageSceneNode::DestroyRecursive() {
    this->ClearResource();
    this->onLoadCallback.Reset();
    this->onErrorCallback.Reset();

    SceneNode::DestroyRecursive();
}

void ImageSceneNode::ClearResource() {
    if (this->image) {
        this->image->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->image);
        this->image = nullptr;
    }
}

void ImageSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on img nodes");
}

} // namespace ls
