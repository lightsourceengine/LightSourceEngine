/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/ImageSceneNode.h>

#include <ls/Style.h>
#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/Renderer.h>
#include <ls/Color.h>
#include <ls/CompositeContext.h>
#include <ls/yoga-ext.h>

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

void ImageSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info);
    this->SetFlag(FlagLeaf, true);
    YGNodeSetMeasureFunc(this->ygNode, SceneNode::YogaMeasureCallback);
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

void ImageSceneNode::OnStylePropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::tintColor:
        case StyleProperty::backgroundColor:
        case StyleProperty::borderColor:
            this->RequestComposite();
            break;
        case StyleProperty::objectFit:
        case StyleProperty::objectPositionX:
        case StyleProperty::objectPositionY:
            this->RequestStyleLayout();
            break;
        default:
            SceneNode::OnStylePropertyChanged(property);
            break;
    }
}

void ImageSceneNode::OnBoundingBoxChanged() {
    this->RequestStyleLayout();
}

void ImageSceneNode::OnStyleLayout() {
    if (this->image && this->image->HasDimensions()) {
        auto bounds{YGNodeLayoutGetInnerRect(this->ygNode)};

        if (!IsEmpty(bounds)) {
            auto fit{this->scene->GetStyleResolver().ResolveObjectFit(this->style, bounds, this->image)};

            this->imageRect = ClipImage(bounds, fit, this->image->WidthF(), this->image->HeightF());
        }
    }

    this->RequestComposite();
}

void ImageSceneNode::Paint(RenderingContext2D* context) {
}

void ImageSceneNode::Composite(CompositeContext* composite) {
    if (this->style == nullptr && (!this->image || this->image->GetState() != Res::Ready)) {
        SceneNode::Composite(composite);
        return;
    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    if (IsEmpty(rect)) {
        SceneNode::Composite(composite);
        return;
    }

    const auto transform{
        composite->CurrentMatrix() * this->scene->GetStyleResolver().ResolveTransform(this->style, rect)
    };
    const auto opacity{ composite->CurrentOpacity() };
    const auto imageStyle{ Style::OrEmpty(this->style) };

    if (!imageStyle->backgroundColor.empty()) {
        composite->renderer->DrawFillRect(
            rect,
            transform,
            imageStyle->backgroundColor.value.MixAlpha(opacity));
    }

    if (this->image) {
        if (!this->image->HasTexture()) {
            this->image->LoadTexture(composite->renderer);
        }

        if (this->image->HasTexture() && !IsEmpty(this->imageRect.dest)) {
            const auto imageDestRect{Translate(this->imageRect.dest, rect.x, rect.y)};

            composite->renderer->DrawImage(this->image->GetTexture(), this->imageRect.src, imageDestRect,
                    transform, imageStyle->tintColor.ValueOr(ColorWhite).MixAlpha(opacity));
        }
    }

    if (!imageStyle->borderColor.empty()) {
        composite->renderer->DrawBorder(
            rect,
            YGNodeLayoutGetBorderRect(this->ygNode),
            transform,
            imageStyle->borderColor.value.MixAlpha(opacity));
    }

    SceneNode::Composite(composite);
}

YGSize ImageSceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    if (this->image && this->image->HasDimensions()) {
        return { this->image->WidthF(), this->image->HeightF() };
    }

    return { 0.f, 0.f };
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
            YGNodeMarkDirty(this->ygNode);
        }
        return;
    }

    if (this->image) {
        YGNodeMarkDirty(this->ygNode);
    }

    this->ClearResource();
    this->src = newSrc;
    this->image = this->GetStage()->GetResources()->AcquireImage(this->src);

    auto listener{ [this](Res::Owner owner, Res* res) {
        constexpr auto LAMBDA_FUNCTION = "ImageResourceListener";

      if (this != owner || this->image != res) {
          LOG_WARN_LAMBDA("Invalid owner or resource: %s", this->src);
          return;
      }

      this->resourceProgress.Dispatch(this, this->image);

      YGNodeMarkDirty(this->ygNode);
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
    return this->resourceProgress.GetOnLoad(info.Env());
}

void ImageSceneNode::SetOnLoadCallback(const CallbackInfo& info, const Napi::Value& value) {
    return this->resourceProgress.SetOnLoad(info.Env(), value);
}

Value ImageSceneNode::GetOnErrorCallback(const CallbackInfo& info) {
    return this->resourceProgress.GetOnError(info.Env());
}

void ImageSceneNode::SetOnErrorCallback(const CallbackInfo& info, const Napi::Value& value) {
    return this->resourceProgress.SetOnLoad(info.Env(), value);
}

void ImageSceneNode::Destroy() {
    this->ClearResource();
    this->resourceProgress.Reset();

    SceneNode::Destroy();
}

void ImageSceneNode::ClearResource() {
    if (this->image) {
        this->image->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->image);
        this->image = nullptr;
    }
}

} // namespace ls
