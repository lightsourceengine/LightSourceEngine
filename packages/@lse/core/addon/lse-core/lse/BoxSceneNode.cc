/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/BoxSceneNode.h>

#include <cstring>
#include <lse/Scene.h>
#include <lse/Stage.h>
#include <lse/Style.h>
#include <lse/CompositeContext.h>
#include <lse/Renderer.h>
#include <lse/Log.h>
#include <lse/Color.h>
#include <lse/yoga-ext.h>
#include <lse/PixelConversion.h>
#include <nanoctx.h>

namespace lse {

BoxSceneNode::BoxSceneNode(napi_env env, Scene* scene) : SceneNode(env, scene) {
}

void BoxSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::backgroundImage:
      this->UpdateBackgroundImage(this->style->GetString(StyleProperty::backgroundImage));
      break;
    case StyleProperty::backgroundColor:
    case StyleProperty::borderColor:
    case StyleProperty::backgroundClip:
    case StyleProperty::backgroundSize:
    case StyleProperty::backgroundPositionX:
    case StyleProperty::backgroundPositionY:
    case StyleProperty::backgroundWidth:
    case StyleProperty::backgroundHeight:
    case StyleProperty::borderRadius:
    case StyleProperty::borderRadiusBottomRight:
    case StyleProperty::borderRadiusBottomLeft:
    case StyleProperty::borderRadiusTopRight:
    case StyleProperty::borderRadiusTopLeft:
      this->RequestStyleLayout();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void BoxSceneNode::OnStyleReset() {
  this->UpdateBackgroundImage(this->style->GetString(StyleProperty::backgroundImage));
  this->RequestStyleLayout();
}

void BoxSceneNode::OnStyleLayout() {
  auto boxStyle{ Style::Or(this->style) };

  if (boxStyle->Exists(StyleProperty::backgroundColor)
      && this->GetStyleContext()->HasBorderRadius(boxStyle)) {
    this->RequestPaint();
    return;
  }

  if (boxStyle->Exists(StyleProperty::backgroundImage)
      && boxStyle->GetEnum(StyleProperty::backgroundRepeat) != StyleBackgroundRepeatOff) {
    this->RequestPaint();
    return;
  }

  // if repeat && background image
  // TODO: if background repeat -> paint

  if (this->backgroundImage && this->backgroundImage->HasDimensions()) {
    const auto backgroundClip{ boxStyle->GetEnum<StyleBackgroundClip>(StyleProperty::backgroundClip) };
    const auto box{ this->GetBackgroundClipBox(backgroundClip) };

    if (!IsEmpty(box)) {
      auto fit{ this->GetStyleContext()->ComputeBackgroundFit(
          this->style, box, this->backgroundImage.get()) };

      this->backgroundImageRect = ClipImage(box, fit,
                                            this->backgroundImage->WidthF(), this->backgroundImage->HeightF());
    }
  }

  this->RequestComposite();
}

void BoxSceneNode::OnBoundingBoxChanged() {
  this->RequestStyleLayout();
}

void BoxSceneNode::Paint(RenderingContext2D* context) {
  const auto boxStyle{ Style::Or(this->style) };

  if (boxStyle->Exists(StyleProperty::backgroundColor)
      && this->GetStyleContext()->HasBorderRadius(boxStyle)) {
    this->PaintBorderRadius(context);
  } else if (this->backgroundImage
      && this->backgroundImage->HasDimensions()
      && boxStyle->Exists(StyleProperty::backgroundImage)
      && boxStyle->GetEnum(StyleProperty::backgroundRepeat) != StyleBackgroundRepeatOff) {
    this->PaintBackgroundRepeat(context);
  }
}

void BoxSceneNode::PaintBorderRadius(RenderingContext2D* context) {
  const auto boxStyle{ Style::Or(this->style) };
  const auto backgroundClip{ boxStyle->GetEnum<StyleBackgroundClip>(StyleProperty::backgroundClip) };
  const auto box{ this->GetBackgroundClipBox(backgroundClip) };
  const auto borderRadius{ this->GetStyleContext()->ComputeBorderRadius(boxStyle, box) };

  if (!this->paintTarget
      || !this->paintTarget.IsLockable()
      || this->paintTarget.Width() != static_cast<int32_t>(box.width)
      || this->paintTarget.Height() != static_cast<int32_t>(box.height)) {
    this->paintTarget.Destroy();
    this->paintTarget = this->scene->GetRenderer()->CreateTexture(box.width, box.height, Texture::Lockable);

    if (!this->paintTarget) {
      LOG_ERROR("Failed to create paint texture.");
      return;
    }
  }

  auto pixels{ this->paintTarget.Lock() };

  // Length proportional to radius of a cubic bezier handle for 90deg arcs.
  constexpr auto KAPPA = 1.f - 0.5522847493f;
  const float x = box.x;
  const float y = box.y;
  const float w = box.width;
  const float h = box.height;
  const float tl = borderRadius.topLeft;
  const float tr = borderRadius.topRight;
  const float bl = borderRadius.bottomLeft;
  const float br = borderRadius.bottomRight;

  auto ctx{nctx_new(pixels.Width(), pixels.Height())};

  // XXX: fix bug in color_t that is causing malformed colors
  auto color{boxStyle->GetColor(StyleProperty::backgroundColor).value_or(0)};

  nctx_set_fill_color(ctx, (color.b << 16) | (color.g << 8) | color.r);
  nctx_begin_path(ctx);

  nctx_move_to(ctx, x+tl, y);
  nctx_line_to(ctx, x+w-tr, y);
  nctx_cubic_bezier_to(ctx, x+w-tr*KAPPA, y, x+w, y+tl*KAPPA, x+w, y+tr);
  nctx_line_to(ctx, x+w, y+h-br);
  nctx_cubic_bezier_to(ctx, x+w, y+h-br*KAPPA, x+w-tr*KAPPA, y+h, x+w-br, y+h);
  nctx_line_to(ctx, x+bl, y+h);
  nctx_cubic_bezier_to(ctx, x+bl*KAPPA, y+h, x, y+h-bl*KAPPA, x, y+h-bl);
  nctx_line_to(ctx, x, y+tl);
  nctx_cubic_bezier_to(ctx, x, y+tl*KAPPA, x+tl*KAPPA, y, x+tl, y);

  nctx_close_path(ctx);
  nctx_fill(ctx);

  nctx_render(ctx, pixels.Data(), pixels.Width(), pixels.Height(), pixels.Pitch());

  nctx_delete(ctx);

  ConvertToFormat(reinterpret_cast<color_t*>(pixels.Data()), pixels.Width() * pixels.Height(), paintTarget.Format());

  pixels.Release();

  this->RequestComposite();
}

void BoxSceneNode::PaintBackgroundRepeat(RenderingContext2D* context) {
  const auto boxStyle{ Style::Or(this->style) };
  const auto backgroundClip{ boxStyle->GetEnum<StyleBackgroundClip>(StyleProperty::backgroundClip) };
  const auto box{ this->GetBackgroundClipBox(backgroundClip) };
  const auto boxWidthI{ static_cast<int32_t>(box.width) };
  const auto boxHeightI{ static_cast<int32_t>(box.height) };

  if (!this->paintTarget || !this->paintTarget.IsLockable()
      || this->paintTarget.Width() != boxWidthI || this->paintTarget.Height() != boxHeightI) {
    this->paintTarget.Destroy();
    this->paintTarget = this->scene->GetRenderer()->CreateTexture(
        boxWidthI, boxHeightI, Texture::RenderTarget);

    if (!this->paintTarget) {
      LOG_ERROR("Failed to create paint texture.");
      return;
    }
  }

  if (!this->backgroundImage->HasTexture() && this->backgroundImage->GetState() == Resource::Ready) {
    if (!this->backgroundImage->LoadTexture(context->renderer)) {
      return;
    }
  }

  auto x{ box.x };
  auto y{ box.y };
  const auto x2{ box.x + box.width };
  const auto y2{ box.y + box.height };
  Rect imageRect{ 0, 0, this->backgroundImage->WidthF(), this->backgroundImage->HeightF() };

  context->renderer->SetRenderTarget(this->paintTarget);
  context->renderer->FillRenderTarget(0);

  switch (boxStyle->GetEnum(StyleProperty::backgroundRepeat)) {
    case StyleBackgroundRepeatXY:
      while (y < y2) {
        x = 0;

        while (x < x2) {
          context->renderer->DrawImage(this->backgroundImage->GetTexture(),
                                       Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
          x += imageRect.width;
        }

        y += imageRect.height;
      }
      break;
    case StyleBackgroundRepeatX:
      while (x < x2) {
        context->renderer->DrawImage(
            this->backgroundImage->GetTexture(), Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
        x += imageRect.width;
      }
      break;
    case StyleBackgroundRepeatY:
      while (y < y2) {
        context->renderer->DrawImage(
            this->backgroundImage->GetTexture(), Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
        y += imageRect.height;
      }
      break;
    default:
      break;
  }

  context->renderer->Reset();

  this->RequestComposite();
}

void BoxSceneNode::Composite(CompositeContext* composite) {
  const auto boxStyle{ this->style.Get() };

  if (boxStyle == nullptr /* TODO: || boxStyle->IsLayoutOnly() */) {
    return;
  }

  const auto box{ YGNodeGetBox(this->ygNode, 0, 0) };
  const auto& transform{ composite->CurrentMatrix() };

  if (this->paintTarget) {
    composite->renderer->DrawImage(this->paintTarget, box, transform,
                                   ColorWhite.MixAlpha(composite->CurrentOpacity()));
    return;
  }

  auto styleColor{ boxStyle->GetColor(StyleProperty::backgroundColor) };

  if (styleColor.has_value()) {
    composite->renderer->DrawFillRect(
        box,
        transform,
        styleColor->MixAlpha(composite->CurrentOpacity()));
  }

  if (this->backgroundImage) {
    if (!this->backgroundImage->HasTexture()) {
      this->backgroundImage->LoadTexture(composite->renderer);

      if (!this->backgroundImage) {
        LOG_ERROR("Failed to create background image texture.");
        return;
      }
    }

    if (this->backgroundImage->HasTexture() && !IsEmpty(this->backgroundImageRect.dest)) {
      const auto backgroundImageDestRect{ Translate(this->backgroundImageRect.dest, box.x, box.y) };

      styleColor = boxStyle->GetColor(StyleProperty::tintColor);

      composite->renderer->DrawImage(
          this->backgroundImage->GetTexture(),
          this->backgroundImageRect.src,
          backgroundImageDestRect,
          transform,
          styleColor.value_or(ColorWhite).MixAlpha(composite->CurrentOpacity()));
    }
  }

  styleColor = boxStyle->GetColor(StyleProperty::borderColor);

  if (styleColor.has_value()) {
    composite->renderer->DrawBorder(box, YGNodeGetBorderEdges(this->ygNode), transform,
                                    styleColor->MixAlpha(composite->CurrentOpacity()));
  }
}

void BoxSceneNode::UpdateBackgroundImage(const std::string& backgroundUri) {
  auto clearBackgroundImageResource = [](BoxSceneNode* node) {
    if (node->backgroundImage && node->backgroundImage->HasTexture()) {
      node->RequestComposite();
    }
    node->ClearBackgroundImageResource();
  };

  clearBackgroundImageResource(this);

  if (backgroundUri.empty()) {
    return;
  }

  this->backgroundImage = this->GetResources()->AcquireImage(backgroundUri);

  auto listener{ [this](Resource::Owner owner, Resource* res) {
    if (this != owner || this->backgroundImage.get() != res) {
      return;
    }

    res->RemoveListener(owner);
    this->RequestStyleLayout();
  }};

  switch (this->backgroundImage->GetState()) {
    case Resource::State::Init:
      this->backgroundImage->AddListener(this, listener);
      this->backgroundImage->Load(this->env);
      break;
    case Resource::State::Loading:
      this->backgroundImage->AddListener(this, listener);
      break;
    case Resource::State::Ready:
    case Resource::State::Error:
      listener(this, this->backgroundImage.get());
      break;
  }
}

void BoxSceneNode::ClearBackgroundImageResource() {
  if (this->backgroundImage) {
    auto resource = this->backgroundImage.get();
    this->backgroundImage->RemoveListener(this);
    this->backgroundImage = nullptr;

    this->GetResources()->ReleaseResource(resource);
  }
}

Rect BoxSceneNode::GetBackgroundClipBox(StyleBackgroundClip value) const noexcept {
  switch (value) {
    case StyleBackgroundClipBorderBox:
      return YGNodeGetBox(this->ygNode, 0, 0);
    case StyleBackgroundClipPaddingBox:
      return YGNodeGetBorderBox(this->ygNode);
    case StyleBackgroundClipContentBox:
      return YGNodeGetPaddingBox(this->ygNode);
  }

  return {};
}

void BoxSceneNode::Destroy() {
  this->paintTarget.Destroy();
  this->ClearBackgroundImageResource();
  SceneNode::Destroy();
}

} // namespace lse
