/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageSceneNode.h"

#include <lse/Image.h>
#include <lse/Style.h>
#include <lse/Scene.h>
#include <lse/Renderer.h>
#include <lse/Color.h>
#include <lse/CompositeContext.h>
#include <lse/yoga-ext.h>

namespace lse {

ImageSceneNode::ImageSceneNode(Scene* scene) : SceneNode(scene) {
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
  if (!Image::SafeIsReady(this->image)) {
    return;
  }

  // Image must consider padding of the scene node.
  const auto bounds{ YGNodeGetPaddingBox(this->ygNode) };

  // Determine where the image should be placed relative to this node's bounds. The placement
  // can overflow the scene node's bounds.
  auto imageDest{this->GetStyleContext()->ComputeObjectFit(
      Style::Or(this->style), bounds, this->image)};

  // Clip the image (destination and source texture coordinates) against the scene node's bounds,
  // image placement and padding.
  this->imageRect = ClipImage(
      bounds, imageDest, this->image->WidthF(), this->image->HeightF());

  this->MarkCompositeDirty();
}

void ImageSceneNode::OnComposite(CompositeContext* ctx) {
  this->DrawBackground(ctx, StyleBackgroundClipBorderBox);

  if (Image::SafeIsReady(this->image)) {
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

void ImageSceneNode::SetSource(const ImageRequest& request) noexcept {
  if (this->image && this->image->GetRequest() == request) {
    return;
  }

  auto imageManager{this->GetImageManager()};

  if (this->image) {
    YGNodeMarkDirty(this->ygNode);
  }

  ImageManager::SafeRelease(imageManager, this->image, this);
  this->image = ImageManager::SafeAcquire(imageManager, request, this, &ImageSceneNode::ImageStatusListener);
}

void ImageSceneNode::ResetSource() noexcept {
  if (this->image) {
    YGNodeMarkDirty(this->ygNode);
  }

  this->image = ImageManager::SafeRelease(this->GetImageManager(), this->image, this);
}

bool ImageSceneNode::HasImageStatusCallback() const noexcept {
  return this->imageStatusCallback != nullptr;
}

void ImageSceneNode::SetImageStatusCallback(std::unique_ptr<ImageStatusCallback>&& callback) noexcept {
  this->imageStatusCallback = std::move(callback);
}

void ImageSceneNode::OnDestroy() {
  this->image = ImageManager::SafeRelease(this->GetImageManager(), this->image, this);
  this->imageStatusCallback = nullptr;
}

void ImageSceneNode::ImageStatusListener(void* owner, Image* image) noexcept {
  auto node{static_cast<ImageSceneNode*>(owner)};

  switch (image->GetState()) {
    case ImageState::Ready:
    case ImageState::Error:
      if (node->imageStatusCallback) {
        node->imageStatusCallback->Invoke(image);
      }
      image->RemoveListener(owner);
      YGNodeMarkDirty(node->ygNode);
      break;
    default:
      break;
  }
}

} // namespace lse
