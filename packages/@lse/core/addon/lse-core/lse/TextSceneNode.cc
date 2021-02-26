/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/TextSceneNode.h>

#include <lse/Scene.h>
#include <lse/yoga-ext.h>
#include <lse/Style.h>
#include <lse/CompositeContext.h>
#include <lse/Log.h>
#include <lse/Timer.h>
#include <lse/Renderer.h>
#include <lse/PixelConversion.h>
#include <lse/RenderingContext2D.h>

namespace lse {

TextSceneNode::TextSceneNode(Scene* scene) : SceneNode(scene) {
  YGNodeSetMeasureFunc(this->ygNode, SceneNode::YogaMeasureCallback);
  YGNodeSetNodeType(this->ygNode, YGNodeTypeText);
}

void TextSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::fontFamily:
    case StyleProperty::fontSize:
    case StyleProperty::fontStyle:
    case StyleProperty::fontWeight:
    case StyleProperty::lineHeight:
    case StyleProperty::maxLines:
    case StyleProperty::textOverflow:
    case StyleProperty::textTransform:
    case StyleProperty::textAlign:
      this->MarkComputeStyleDirty();
      break;
    case StyleProperty::borderColor: // TODO: borderColor?
    case StyleProperty::color:
      this->MarkCompositeDirty();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void TextSceneNode::OnFlexBoxLayoutChanged() {
  this->MarkCompositeDirty();
}

void TextSceneNode::OnComputeStyle() {
  if (this->SetFont(this->style)) {
     this->block.Invalidate();
     YGNodeMarkDirty(this->ygNode);
  }
}

YGSize TextSceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  if (widthMode == YGMeasureModeUndefined) {
    width = this->scene->GetWidth();
  }

  if (heightMode == YGMeasureModeUndefined) {
    height = this->scene->GetHeight();
  }

  this->block.Shape(this->text, this->fontFace, this->style, this->GetStyleContext(), width, height);

  return { this->block.WidthF(), this->block.HeightF() };
}

void TextSceneNode::OnComposite(CompositeContext* ctx) {
  this->DrawBackground(ctx, StyleBackgroundClipBorderBox);
  this->DrawText(ctx);
  this->DrawBorder(ctx);
}

const std::string& TextSceneNode::GetText() const {
  return this->text;
}

void TextSceneNode::SetText(std::string&& text) {
  if (this->text != text) {
    this->text = std::move(text);
    this->block.Invalidate();
    YGNodeMarkDirty(this->ygNode);
  }
}

bool TextSceneNode::SetFont(Style* style) {
  auto dirty{ false };

  if (!style) {
    if (this->fontFace) {
      dirty = true;
    }

    this->ClearFontFaceResource();

    return dirty;
  }

  auto findFontResult = this->scene->GetFontManager()->FindFont(
      style->GetString(StyleProperty::fontFamily),
      style->GetEnum<StyleFontStyle>(StyleProperty::fontStyle),
      style->GetEnum<StyleFontWeight>(StyleProperty::fontWeight));

  if (this->fontFace == findFontResult) {
    return false;
  }

  // TODO: ResetFont() ?
  this->ClearFontFaceResource();
  this->fontFace = findFontResult;

  if (!this->fontFace) {
    return true;
  }

  switch (this->fontFace->GetFontStatus()) {
    case FontStatusReady:
      dirty = true;
      break;
    case FontStatusLoading:
      this->fontFace->AddListener(this, [](Font* font, void* listener, FontStatus status) {
        auto self{static_cast<TextSceneNode*>(listener)};
        if (status == FontStatusReady) {
          self->block.Invalidate();
          YGNodeMarkDirty(self->ygNode);
          font->RemoveListener(listener);
        } else {
          self->ClearFontFaceResource();
        }
      });
      break;
    default:
      // TODO: clear?
      this->ClearFontFaceResource();
      dirty = true;
      break;
  }

  return dirty;
}

void TextSceneNode::DrawText(CompositeContext* ctx) {
  this->block.Paint(this->scene->GetRenderingContext2D());

  if (!this->block.IsReady()) {
    return;
  }

  auto box{YGNodeGetPaddingBox(this->ygNode)};
  auto boxStyle{Style::Or(this->style)};
  Rect pos{
    box.x + ctx->CurrentMatrix().GetTranslateX(),
    box.y + ctx->CurrentMatrix().GetTranslateY(),
    this->block.WidthF(),
    this->block.HeightF()
  };

  switch (boxStyle->GetEnum(StyleProperty::textAlign)) {
    case StyleTextAlignCenter:
      pos.x += ((box.width - pos.width) / 2.f);
      break;
    case StyleTextAlignRight:
      pos.x += (box.width - pos.width);
      break;
    case StyleTextAlignLeft:
    default:
      break;
  }

  // TODO: clip

  auto textColor{ boxStyle->GetColor(StyleProperty::color).value_or(ColorBlack) };

  ctx->renderer->DrawImage(
      pos,
      {0, 0, this->block.GetTexture()->Width(), this->block.GetTexture()->Height()},
      this->block.GetTexture(),
      RenderFilter::OfTint(textColor.MixAlpha(ctx->CurrentOpacity())));
}

void TextSceneNode::ClearFontFaceResource() {
  if (this->fontFace) {
    this->fontFace->RemoveListener(this);
    this->fontFace = nullptr;
  }
}

void TextSceneNode::OnDestroy() {
  this->ClearFontFaceResource();
  this->block.Destroy();
}

// TODO: temporary hack due to scene and renderer shutdown conflicts
void TextSceneNode::OnDetach() {
  this->ClearFontFaceResource();
  this->block.Destroy();
}

} // namespace lse
