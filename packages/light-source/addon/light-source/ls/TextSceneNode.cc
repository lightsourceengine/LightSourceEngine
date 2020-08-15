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
#include <ls/Surface.h>
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
    if (!this->SetFont(Style::OrEmpty(this->style))) {
//        this->block.Invalidate();
        // YGNodeMarkDirty(this->ygNode);
        // TODO: shape?
    }

    this->RequestPaint();
}

YGSize TextSceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    this->block.Shape(this->text, this->fontFace, this->style, this->scene->GetStyleResolver(), width, height);

    return { this->block.WidthF(), this->block.HeightF() };
}

void TextSceneNode::Paint(RenderingContext2D* context) {
    this->block.Paint(context, this->scene->GetRenderer());
    this->RequestComposite();
}

void TextSceneNode::Composite(CompositeContext* composite) {
    const auto boxStyle{ this->style };

//    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
//        SceneNode::Composite(composite);
//        return;
//    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    if (IsEmpty(rect)) {
        SceneNode::Composite(composite);
        return;
    }

    const auto transform{
        composite->CurrentMatrix() * this->scene->GetStyleResolver().ResolveTransform(boxStyle, rect)
    };

    if (!this->block.IsEmpty()) { // TODO: has texture?, check color
        Rect pos{ rect.x, rect.y, this->block.WidthF(), this->block.HeightF() };

        composite->renderer->DrawImage(this->block.GetTexture(), pos, transform,
               boxStyle->color.ValueOr(ColorBlack).MixAlpha(composite->CurrentOpacity()));
        SceneNode::Composite(composite);
        return;
    }

    if (!boxStyle->borderColor.empty()) {
        composite->renderer->DrawBorder(
            rect,
            YGNodeLayoutGetBorderRect(this->ygNode),
            transform,
            boxStyle->borderColor.value.MixAlpha(composite->CurrentOpacity()));
    }

    SceneNode::Composite(composite);
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

    if (!style || style->fontFamily.empty() || style->fontSize.empty()) {
        if (this->fontFace) {
            dirty = true;
        }

        this->ClearFontFaceResource();

        return dirty;
    }

    if (FontFace::Equals(this->fontFace, style->fontFamily, style->fontStyle, style->fontWeight)) {
        return false;
    }

    this->ClearFontFaceResource();
    this->fontFace = this->GetStage()->GetResources()->AcquireFontFaceByStyle(
        style->fontFamily, style->fontStyle, style->fontWeight);

    if (!this->fontFace) {
        return true;
    }

    switch (this->fontFace->GetState()) {
        case Res::State::Ready:
            dirty = true;
            break;
        case Res::State::Loading:
            this->fontFace->AddListener(this, [this](Res::Owner owner, Res* res) {
              if (this != owner || this->fontFace != res) {
                  return;
              }

              if (this->fontFace->GetState() == Res::State::Ready) {
                  this->block.Invalidate();
                  YGNodeMarkDirty(this->ygNode);
                  this->fontFace->RemoveListener(owner);
              } else {
                  this->ClearFontFaceResource();
              }
            });
            break;
        default:
            this->ClearFontFaceResource();
            dirty = true;
            break;
    }

    return dirty;
}

void TextSceneNode::ClearFontFaceResource() {
    if (this->fontFace) {
        this->fontFace->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->fontFace);
        this->fontFace = nullptr;
    }
}

void TextSceneNode::Destroy() {
    this->ClearFontFaceResource();

    SceneNode::Destroy();
}

} // namespace ls
