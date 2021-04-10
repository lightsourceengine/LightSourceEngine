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

#include <lse/TextSceneNode.h>

#include <lse/Scene.h>
#include <lse/yoga-ext.h>
#include <lse/Style.h>
#include <lse/CompositeContext.h>
#include <lse/Log.h>
#include <lse/Timer.h>
#include <lse/Renderer.h>
#include <lse/PixelConversion.h>

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
      this->MarkComputeStyleDirty();
      break;
    case StyleProperty::textAlign:
    case StyleProperty::borderColor: // TODO: borderColor?
    case StyleProperty::color:
    case StyleProperty::filter:
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
  auto box{YGNodeGetPaddingBox(this->ygNode)};
  auto textStyle{Style::Or(this->style)};

  if (this->block.IsEmpty()) {
    // if style width and height are set, measure is not used. if this is the situation, shape the text before paint.
    this->block.Shape(this->text, this->fontFace, textStyle, this->GetStyleContext(), box.width, box.height);
  }

  this->block.Paint(ctx->renderer);

  if (!this->block.IsReady()) {
    return;
  }

  Rect pos{
    box.x + ctx->CurrentMatrix().GetTranslateX(),
    box.y + ctx->CurrentMatrix().GetTranslateY(),
    this->block.WidthF(),
    this->block.HeightF()
  };

  switch (textStyle->GetEnum(StyleProperty::textAlign)) {
    case StyleTextAlignCenter:
      pos.x += ((box.width - pos.width) / 2.f);
      break;
    case StyleTextAlignRight:
      pos.x += (box.width - pos.width);
      break;
    default:
      break;
  }

  // TODO: clip

  auto textColor{ textStyle->GetColor(StyleProperty::color).value_or(ColorBlack) };

  ctx->renderer->DrawImage(
      pos,
      this->block.GetTextureSourceRect(),
      this->block.GetTexture(),
      this->GetStyleContext()->ComputeFilter(textStyle, textColor, ctx->CurrentOpacity()));
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
