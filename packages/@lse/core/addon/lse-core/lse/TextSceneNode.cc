/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/TextSceneNode.h>

#include <ls/Stage.h>
#include <ls/Scene.h>
#include <ls/yoga-ext.h>
#include <ls/Style.h>
#include <ls/CompositeContext.h>
#include <ls/Timer.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

static const std::string kDefaultFontFamily = "default";

void TextSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info);
    this->SetFlag(FlagLeaf, true);
    YGNodeSetMeasureFunc(this->ygNode, SceneNode::YogaMeasureCallback);
    YGNodeSetNodeType(this->ygNode, YGNodeTypeText);
}

Function TextSceneNode::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(
            env,
            "TextSceneNode", true,
            SceneNode::Extend<TextSceneNode>(env, {
                InstanceAccessor("text", &TextSceneNode::GetText, &TextSceneNode::SetText),
            }));
    }

    return constructor.Value();
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
    if (!this->SetFont(this->style.get())) {
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

    this->block.Shape(this->text, this->fontFace, this->style.get(), this->GetStyleContext(), width, height);

    return { this->block.WidthF(), this->block.HeightF() };
}

void TextSceneNode::Paint(RenderingContext2D* context) {
    this->block.Paint(context);
    this->RequestComposite();
}

void TextSceneNode::Composite(CompositeContext* composite) {
    const auto boxStyle{Style::Or(this->style) };

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

Value TextSceneNode::GetText(const CallbackInfo& info) {
    return String::New(info.Env(), this->text);
}

void TextSceneNode::SetText(const CallbackInfo& info, const Napi::Value& value) {
    std::string str;

    if (value.IsString()) {
        str = value.As<String>();
    } else if (value.IsNull() || value.IsUndefined()) {
        str = "";
    } else {
        throw Error::New(info.Env(), "Cannot assign non-string value to text property.");
    }

    if (this->text != str) {
        this->text = str;
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

    const auto& fontFamily{
        style->IsEmpty(StyleProperty::fontFamily) ? kDefaultFontFamily : style->GetString(StyleProperty::fontFamily)
    };
    const auto fontStyle{ style->GetEnum<StyleFontStyle>(StyleProperty::fontStyle) };
    const auto fontWeight{ style->GetEnum<StyleFontWeight>(StyleProperty::fontWeight) };

    if (FontFace::Equals(this->fontFace, fontFamily, fontStyle, fontWeight)) {
        return false;
    }

    this->ClearFontFaceResource();
    this->fontFace = this->GetResources()->AcquireFontFaceByStyle(fontFamily, fontStyle, fontWeight);

    if (!this->fontFace) {
        return true;
    }

    switch (this->fontFace->GetState()) {
        case Resource::State::Ready:
            dirty = true;
            break;
        case Resource::State::Loading:
            this->fontFace->AddListener(this, [this](Resource::Owner owner, Resource* res) {
              if (this != owner || this->fontFace.get() != res) {
                  return;
              }

              if (this->fontFace->GetState() == Resource::State::Ready) {
                  this->block.Invalidate();
                  YGNodeMarkDirty(this->ygNode);
                  this->fontFace->RemoveListener(owner);
              } else {
                  // TODO: clear?
                  this->ClearFontFaceResource();
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
        auto resource = this->fontFace.get();
        this->fontFace->RemoveListener(this);
        this->fontFace = nullptr;

        this->GetResources()->ReleaseResource(resource);
    }
}

void TextSceneNode::Destroy() {
    this->ClearFontFaceResource();

    SceneNode::Destroy();
}

} // namespace ls
