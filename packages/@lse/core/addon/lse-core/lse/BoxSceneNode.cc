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

#include "BoxSceneNode.h"

#include <lse/Image.h>
#include <lse/Scene.h>
#include <lse/Style.h>
#include <lse/CompositeContext.h>
#include <lse/Renderer.h>

namespace lse {

BoxSceneNode::BoxSceneNode(Scene* scene) : SceneNode(scene) {
}

void BoxSceneNode::OnComputeStyle() {
  auto boxStyle{Style::Or(this->style)};
  bool release{};
  bool acquire{};

  if (boxStyle->IsEmpty(StyleProperty::backgroundImage)) {
    release = true;
  } else if (this->backgroundImage) {
    if (this->backgroundImage->GetRequest().uri != boxStyle->GetString(StyleProperty::backgroundImage)) {
      release = acquire = true;
    }
  } else {
    acquire = true;
  }

  if (release) {
    this->backgroundImage = ImageManager::SafeRelease(this->GetImageManager(), this->backgroundImage, this);
  }

  if (acquire) {
    this->backgroundImage = ImageManager::SafeAcquire(
        this->GetImageManager(),
        {boxStyle->GetString(StyleProperty::backgroundImage)},
        this,
        &BoxSceneNode::ImageStatusListener);
  }

  if (Image::SafeIsReady(this->backgroundImage)) {
    auto bounds = YGNodeGetBox(this->ygNode, 0, 0);
    auto dest = this->GetStyleContext()->ComputeBackgroundFit(
        boxStyle,
        bounds,
        this->backgroundImage);

    this->backgroundImageRect = ClipImage(
        bounds,
        dest,
        this->backgroundImage->WidthF(),
        this->backgroundImage->HeightF());
  } else {
    this->backgroundImageRect = {};
  }

  this->MarkCompositeDirty();
}

void BoxSceneNode::OnComposite(CompositeContext* ctx) {
  auto boxStyle{Style::Or(this->style)};

  this->DrawBackground(ctx, boxStyle->GetEnum<StyleBackgroundClip>(StyleProperty::backgroundClip));

  if (Image::SafeIsReady(this->backgroundImage) && !IsEmpty(this->backgroundImageRect)) {
    auto repeat{boxStyle->GetEnum<StyleBackgroundRepeat>(StyleProperty::backgroundRepeat)};

    if (repeat != StyleBackgroundRepeatOff) {
      this->DrawBackgroundImageRepeat(ctx, repeat);
    } else {
      ctx->renderer->DrawImage(
          Translate(
              this->backgroundImageRect.dest,
              ctx->CurrentMatrix().GetTranslateX(),
              ctx->CurrentMatrix().GetTranslateY()),
          this->backgroundImageRect.src,
          this->backgroundImage->GetTexture(),
          {});
    }
  }

  this->DrawBorder(ctx);
}

void BoxSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::backgroundImage:
    case StyleProperty::backgroundRepeat:
    case StyleProperty::backgroundClip:
    case StyleProperty::backgroundSize:
    case StyleProperty::backgroundPositionX:
    case StyleProperty::backgroundPositionY:
    case StyleProperty::backgroundWidth:
    case StyleProperty::backgroundHeight:
      this->MarkComputeStyleDirty();
      break;
    case StyleProperty::backgroundColor:
    case StyleProperty::borderColor:
      this->MarkCompositeDirty();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void BoxSceneNode::OnFlexBoxLayoutChanged() {
  this->MarkComputeStyleDirty();
}

void BoxSceneNode::OnDestroy() {
  this->backgroundImage = ImageManager::SafeRelease(this->GetImageManager(), this->backgroundImage, this);
}

void BoxSceneNode::ImageStatusListener(void* owner, Image* image) noexcept {
  auto node{static_cast<BoxSceneNode*>(owner)};

  switch (image->GetState()) {
    case ImageState::Ready:
    case ImageState::Error:
      image->RemoveListener(owner);
      node->MarkComputeStyleDirty();
      break;
    default:
      break;
  }
}

void BoxSceneNode::DrawBackgroundImageRepeat(CompositeContext* ctx, StyleBackgroundRepeat repeat) {
  // TODO: x and y can be configured by the user. position needs to be updated so repeat covers renderable area
  auto x{this->backgroundImageRect.dest.x + ctx->CurrentMatrix().GetTranslateX()};
  auto y{this->backgroundImageRect.dest.y + ctx->CurrentMatrix().GetTranslateY()};
  auto x2{x + YGNodeLayoutGetWidth(this->ygNode)};
  auto y2{x + YGNodeLayoutGetHeight(this->ygNode)};

  switch (repeat) {
    case StyleBackgroundRepeatXY:
      while (y < y2) {
        x = 0;

        while (x < x2) {
          ctx->renderer->DrawImage(
              {x, y, this->backgroundImageRect.dest.width, this->backgroundImageRect.dest.height},
              this->backgroundImageRect.src,
              this->backgroundImage->GetTexture(),
              {});
          x += this->backgroundImageRect.dest.width;
        }

        y += this->backgroundImageRect.dest.height;
      }
      break;
    case StyleBackgroundRepeatX:
      while (x < x2) {
        ctx->renderer->DrawImage(
            {x, y, this->backgroundImageRect.dest.width, this->backgroundImageRect.dest.height},
            this->backgroundImageRect.src,
            this->backgroundImage->GetTexture(),
            {});
        x += this->backgroundImageRect.dest.width;
      }
      break;
    case StyleBackgroundRepeatY:
      while (y < y2) {
        ctx->renderer->DrawImage(
            {x, y, this->backgroundImageRect.dest.width, this->backgroundImageRect.dest.height},
            this->backgroundImageRect.src,
            this->backgroundImage->GetTexture(),
            {});
        y += this->backgroundImageRect.dest.height;
      }
      break;
    default:
      break;
  }
}

} // namespace lse
