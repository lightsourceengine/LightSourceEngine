/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/ImageSceneNode.h>

// TODO: remove after image loading depends are removed
#include <napix.h>

#include <lse/Style.h>
#include <lse/Scene.h>
#include <lse/Stage.h>
#include <lse/Renderer.h>
#include <lse/Color.h>
#include <lse/CompositeContext.h>
#include <lse/yoga-ext.h>

namespace lse {

ImageSceneNode::ImageSceneNode(napi_env env, Scene* scene) : SceneNode(env, scene) {
  this->SetFlag(FlagLeaf, true);
  YGNodeSetMeasureFunc(this->ygNode, SceneNode::YogaMeasureCallback);
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

const std::string& ImageSceneNode::GetSource() const noexcept {
  return this->src;
}

void ImageSceneNode::SetSource(napi_env env, std::string&& value) noexcept {
  if (value == this->src) {
    return;
  }

  if (value.empty()) {
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
  this->src = std::move(value);
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

napi_value ImageSceneNode::GetOnLoadCallback(napi_env env) noexcept {
  try {
    return this->resourceProgress.GetOnLoad(env);
  } catch (const Napi::Error& e) {
    napix::throw_error(env, e.what());
  }

  return {};
}

void ImageSceneNode::SetOnLoadCallback(napi_env env, napi_value value) noexcept {
  try {
    this->resourceProgress.SetOnLoad(env, Napi::Value(env, value));
  } catch (const Napi::Error& e) {
    napix::throw_error(env, e.what());
  }
}

napi_value ImageSceneNode::GetOnErrorCallback(napi_env env) noexcept {
  try {
    return this->resourceProgress.GetOnError(env);
  } catch (const Napi::Error& e) {
    napix::throw_error(env, e.what());
  }

  return {};
}

void ImageSceneNode::SetOnErrorCallback(napi_env env, napi_value value) noexcept {
  try {
    this->resourceProgress.SetOnError(env, Napi::Value(env, value));
  } catch (const Napi::Error& e) {
    napix::throw_error(env, e.what());
  }
}

void ImageSceneNode::Destroy() {
  this->ClearResource();
  this->resourceProgress.Reset();

  SceneNode::Destroy();
}

// TODO: temporary hack due to scene and renderer shutdown conflicts
void ImageSceneNode::OnDetach() {
  this->ClearResource();
  this->resourceProgress.Reset();
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
