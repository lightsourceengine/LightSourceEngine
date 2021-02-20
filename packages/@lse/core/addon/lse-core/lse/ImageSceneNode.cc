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
  // set up the yoga node to call SceneNode::OnMeasure() when a measure is requested.
  YGNodeSetMeasureFunc(this->ygNode, SceneNode::YogaMeasureCallback);
}

void ImageSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::objectFit:
    case StyleProperty::objectPositionX:
    case StyleProperty::objectPositionY:
      this->MarkComputeStyleDirty();
      break;
    case StyleProperty::tintColor:
    case StyleProperty::backgroundColor:
    case StyleProperty::borderColor:
      // TODO: maybe paint dirty?
      this->MarkCompositeDirty();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void ImageSceneNode::OnFlexBoxLayoutChanged() {
  this->MarkComputeStyleDirty();
}

void ImageSceneNode::OnComputeStyle() {
  if (!this->image || !this->image->HasTexture()) {
    return;
  }

  // Image must consider padding of the scene node.
  const auto bounds{ YGNodeGetPaddingBox(this->ygNode) };

  // Determine where the image should be placed relative to this node's bounds. The placement
  // can overflow the scene node's bounds.
  auto imageDest{this->GetStyleContext()->ComputeObjectFit(
      Style::Or(this->style), bounds, this->image.get())};

  // Clip the image (destination and source texture coordinates) against the scene node's bounds,
  // image placement and padding.
  this->imageRect = ClipImage(
      bounds, imageDest, this->image->WidthF(), this->image->HeightF());

  this->MarkCompositeDirty();
}

void ImageSceneNode::OnComposite(CompositeContext* ctx) {
  this->DrawBackground(ctx, StyleBackgroundClipBorderBox);

  if (this->image && this->image->HasTexture()) {
//    auto box{YGNodeGetBox(this->ygNode)};
    const auto& transform{ ctx->CurrentRenderTransform() };

//    box.width *= ctx->CurrentMatrix().GetScaleX();
//    box.height *= ctx->CurrentMatrix().GetScaleY();

    const auto filter{RenderFilter::OfTint(
        Style::Or(this->style)->GetColor(StyleProperty::tintColor).value_or(ColorWhite),
        ctx->CurrentOpacity())};

    ctx->renderer->DrawImage(
        transform,
        { 0, 0 },
        this->imageRect.dest,
        this->imageRect.src,
        this->image->GetTexture(),
        filter);
  }

  this->DrawBorder(ctx);
}

YGSize ImageSceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  if (this->image && this->image->HasDimensions()) {
    return { this->image->WidthF(), this->image->HeightF() };
  }

  return SceneNode::OnMeasure(width, widthMode, height, heightMode);
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

    if (this->image->GetState() == Resource::Ready && !this->image->HasTexture()) {
      // TODO: is the renderer attached?
      this->image->LoadTexture(this->scene->GetRenderer());
    }

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

void ImageSceneNode::OnDestroy() {
  this->ClearResource();
  this->resourceProgress.Reset();
}

// TODO: temporary hack due to scene and renderer shutdown conflicts
void ImageSceneNode::OnDetach() {
  this->ClearResource();
  this->resourceProgress.Reset();
}

void ImageSceneNode::ClearResource() {
  // TODO: releasing a resource should be less verbose
  if (this->image) {
    auto resource = this->image.get();
    this->image->RemoveListener(this);
    this->image = nullptr;

    this->GetResources()->ReleaseResource(resource);
  }
}

} // namespace lse
