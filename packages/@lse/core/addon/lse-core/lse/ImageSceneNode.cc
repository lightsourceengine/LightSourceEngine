/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/ImageSceneNode.h>

#include <lse/Style.h>
#include <lse/Scene.h>
#include <lse/Stage.h>
#include <lse/Renderer.h>
#include <lse/Color.h>
#include <lse/CompositeContext.h>
#include <lse/yoga-ext.h>

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

namespace lse {

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
  // TODO: maybe check image rect size change to reduce unnecessary composites
  // TODO: probably need to clear imageRect on src change
  if (this->image && this->image->HasDimensions()) {
    const auto bounds{ YGNodeGetPaddingBox(this->ygNode) };

    if (!IsEmpty(bounds)) {
      this->imageRect = ClipImage(
          bounds,
          this->GetStyleContext()->ComputeObjectFit(Style::Or(this->style), bounds, this->image.get()),
          this->image->WidthF(),
          this->image->HeightF());
    }
  }

  this->RequestComposite();
}

void ImageSceneNode::Paint(RenderingContext2D* context) {
}

void ImageSceneNode::Composite(CompositeContext* composite) {
  const auto& transform{ composite->CurrentMatrix() };
  const auto opacity{ composite->CurrentOpacity() };
  const auto imageStyle{ Style::Or(this->style) };
  auto styleColor{ imageStyle->GetColor(StyleProperty::backgroundColor) };

  if (styleColor.has_value()) {
    composite->renderer->DrawFillRect(YGNodeGetBorderBox(this->ygNode), transform, styleColor->MixAlpha(opacity));
  }

  if (this->image && this->image->GetState() == Resource::Ready && !IsEmpty(this->imageRect.dest)) {
    if (!this->image->HasTexture()) {
      this->image->LoadTexture(composite->renderer);
    }

    styleColor = imageStyle->GetColor(StyleProperty::tintColor);

    composite->renderer->DrawImage(
        this->image->GetTexture(),
        this->imageRect.src,
        this->imageRect.dest,
        transform,
        styleColor.value_or(ColorWhite).MixAlpha(opacity));
  }

  styleColor = imageStyle->GetColor(StyleProperty::borderColor);

  if (styleColor.has_value()) {
    composite->renderer->DrawBorder(
        YGNodeGetBox(this->ygNode, 0, 0),
        YGNodeGetBorderEdges(this->ygNode),
        transform,
        styleColor->MixAlpha(opacity));
  }
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
      LOG_WARN("src must be a string")
      return;
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
  this->image = this->GetResources()->AcquireImage(this->src);

  auto listener{ [this](Resource::Owner owner, Resource* res) {
    constexpr auto LAMBDA_FUNCTION = "ImageResourceListener";

    if (this != owner || this->image.get() != res) {
      LOG_WARN_LAMBDA("Invalid owner or resource: %s", this->src);
      return;
    }

    this->resourceProgress.Dispatch(this, this->image.get());

    YGNodeMarkDirty(this->ygNode);
    res->RemoveListener(owner);
  }};

  switch (this->image->GetState()) {
    case Resource::State::Init:
      this->image->AddListener(this, listener);
      this->image->Load(env);
      break;
    case Resource::State::Loading:
      this->image->AddListener(this, listener);
      break;
    case Resource::State::Ready:
    case Resource::State::Error:
      // TODO: should Dispatch() run callbacks synchronously or through a microtask?
      listener(this, this->image.get());
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
  return this->resourceProgress.SetOnError(info.Env(), value);
}

void ImageSceneNode::Destroy() {
  this->ClearResource();
  this->resourceProgress.Reset();

  SceneNode::Destroy();
}

void ImageSceneNode::ClearResource() {
  if (this->image) {
    auto resource = this->image.get();
    this->image->RemoveListener(this);
    this->image = nullptr;

    this->GetResources()->ReleaseResource(resource);
  }
}

} // namespace lse
