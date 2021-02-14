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
#include <lse/Timer.h>
#include <lse/Renderer.h>
#include <lse/PixelConversion.h>

namespace lse {

TextSceneNode::TextSceneNode(napi_env env, Scene* scene) : SceneNode(env, scene) {
  this->SetFlag(FlagLeaf, true);
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
      this->RequestStyleLayout();
      break;
    case StyleProperty::borderColor: // TODO: borderColor?
    case StyleProperty::color:
      this->RequestComposite();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void TextSceneNode::OnBoundingBoxChanged() {
  this->RequestStyleLayout();
}

void TextSceneNode::OnStyleLayout() {
  // TODO: review logic..
  if (!this->SetFont(this->style)) {
//        this->block.Invalidate();
    // YGNodeMarkDirty(this->ygNode);
    // TODO: shape?
  }

  this->RequestPaint();
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

void TextSceneNode::Paint(RenderingContext2D* context) {
  this->block.Paint(context);
  this->RequestComposite();
}

void TextSceneNode::Composite(CompositeContext* composite) {
  const auto boxStyle{ Style::Or(this->style) };

//    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
//        return;
//    }

  const auto box{ YGNodeGetPaddingBox(this->ygNode) };

  if (IsEmpty(box)) {
    return;
  }

  const auto& transform{ composite->CurrentMatrix() };

  if (!this->block.IsEmpty()) { // TODO: has texture?
    Rect pos{ box.x, box.y, this->block.WidthF(), this->block.HeightF() };

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

    composite->renderer->DrawImage(this->block.GetTexture(), pos, transform,
                                   textColor.MixAlpha(composite->CurrentOpacity()));
  }

  auto styleColor{ boxStyle->GetColor(StyleProperty::borderColor) };

  if (styleColor.has_value()) {
    composite->renderer->DrawBorder(
        YGNodeGetBox(this->ygNode, 0, 0),
        YGNodeGetBorderEdges(this->ygNode),
        transform,
        styleColor->MixAlpha(composite->CurrentOpacity()));
  }
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

void TextSceneNode::ClearFontFaceResource() {
  if (this->fontFace) {
    this->fontFace->RemoveListener(this);
    this->fontFace = nullptr;
  }
}

void TextSceneNode::Destroy() {
  this->ClearFontFaceResource();
  this->block.Destroy();

  SceneNode::Destroy();
}

// TODO: temporary hack due to scene and renderer shutdown conflicts
void TextSceneNode::OnDetach() {
  this->ClearFontFaceResource();
  this->block.Destroy();
}

} // namespace lse
