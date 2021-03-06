/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "ImageSceneNode.h"

#include <lse/Image.h>
#include <lse/Style.h>
#include <lse/Scene.h>
#include <lse/Renderer.h>
#include <lse/Color.h>
#include <lse/Log.h>
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
    case StyleProperty::filter:
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
    const auto& transform{ctx->CurrentRenderTransform()};
    auto imageStyle{Style::Or(this->style)};

//    box.width *= ctx->CurrentMatrix().GetScaleX();
//    box.height *= ctx->CurrentMatrix().GetScaleY();

    ctx->renderer->DrawImage(
        transform,
        { 0, 0 },
        Translate(this->imageRect.dest, ctx->CurrentMatrix().GetTranslateX(), ctx->CurrentMatrix().GetTranslateY()),
        this->imageRect.src,
        this->image->GetTexture(),
        this->GetStyleContext()->ComputeFilter(imageStyle, ColorWhite, ctx->CurrentOpacity()));
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
