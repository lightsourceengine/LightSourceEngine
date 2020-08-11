/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/TextSceneNode.h>

#include <ls/Stage.h>
#include <ls/Scene.h>
#include <ls/Font.h>
#include <ls/StyleUtils.h>
#include <ls/yoga-ext.h>
#include <ls/Style.h>
#include <ls/CompositeContext.h>
#include <ls/Timer.h>
#include <ls/Surface.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>
#include <ls-ctx.h>

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

//static int32_t GetMaxLines(Style* style) noexcept;
//static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text);

void TextSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info);
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
        case StyleProperty::borderColor:
            this->RequestStyleLayout();
            break;
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
    this->RequestComposite();
}

YGSize TextSceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
//            auto self{ YGSceneNodeContext::Cast(YGNodeGetContext(nodeRef))->As<TextSceneNode>() };
//
//            try {
//                return self->Measure(width, widthMode, height, heightMode);
//            } catch (const std::exception& e) {
//                constexpr auto LAMBDA_FUNCTION = "YogaMeasureCallback";
//                LOG_ERROR_LAMBDA("text measure: %s", e);
//            }

    return { 0.f, 0.f };
}

//void TextSceneNode::BeforeLayout() {
//    if (this->SetFont(this->style)) {
//        this->QueueTextLayout();
//    }
//}
//
//void TextSceneNode::AfterLayout() {
//    if (this->layout.IsEmpty()) {
//        try {
//            const auto box{ YGNodeLayoutGetInnerRect(this->ygNode) };
//            this->Measure(box.width, YGMeasureModeExactly, box.height, YGMeasureModeExactly);
//        } catch (const std::exception& e) {
//            LOG_ERROR("text measure: %s", e);
//        }
//    }
//
//    // TODO: Layout change may not need a repaint. If only the position changes, only a composite should be queued.
//    this->QueuePaint();
//}

void TextSceneNode::Paint(GraphicsContext* graphicsContext) {
//    const auto boxStyle{ this->GetStyleOrEmpty() };
//
//    if (!this->font || !this->font->IsReady() || this->layout.IsEmpty() || boxStyle->color.empty()) {
//        return;
//    }
//
//    const auto box{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
//    const auto w{ box.width };
//    const auto h{ box.height };
//
//    // TODO: if layout dimensions are animated, this will create and recreate the texture every frame!
//    // TODO: if no border, color can be a composite field and the layer can be sized to the text layout dimensions,
//    //       rather than the (probably) much larger box dimensions.
//    if (!this->InitLayerSoftwareRenderTarget(paint->renderer, static_cast<int32_t>(w), static_cast<int32_t>(h))) {
//        return;
//    }
//
//    const auto surface{ this->layer->Lock() };
//    // TODO: do not allocate on every repaint!
//    // TODO: texture format -> ctx format
//    const auto inner{ YGNodeLayoutGetInnerRect(this->ygNode) };
//    const auto fontSize{ ComputeFontSize(boxStyle->fontSize, this->scene) };
//    const auto lineHeight{ ComputeLineHeight(boxStyle->lineHeight, this->scene,
//        this->font->GetFont()->LineHeight(fontSize)) };
//    const auto textAlign{ boxStyle->textAlign };
//    auto xpos{ 0.f };
//    auto ypos{ inner.y + this->font->GetFont()->Ascent(fontSize) };
//    const auto hasBorderColor{ !boxStyle->borderColor.empty() };
//    auto ctx{ paint->Context2D(surface) };
//
//    ctx_set_font(ctx, this->font->GetId().c_str());
//    ctx_set_font_size(ctx, fontSize);
//
//    if (hasBorderColor) {
//        const auto color{ boxStyle->color.value };
//        ctx_set_rgba_u8(ctx, GetR(color), GetG(color), GetB(color), GetA(color));
//    } else {
//        ctx_set_rgba_u8(ctx, GetR(ColorWhite), GetG(ColorWhite), GetB(ColorWhite), GetA(ColorWhite));
//    }
//
//    surface.FillTransparent();
//
//    for (auto& textLine : this->layout.lines) {
//        switch (textAlign) {
//            case StyleTextAlignLeft:
//                xpos = inner.x;
//                break;
//            case StyleTextAlignCenter:
//                xpos = inner.x + ((inner.width - textLine.width) / 2.f);
//                break;
//            case StyleTextAlignRight:
//                xpos = inner.x + inner.width - textLine.width;
//                break;
//        }
//
//        ctx_move_to(ctx, xpos, ypos);
//        ctx_text_w(ctx, textLine.chars.data(), textLine.chars.size());
//
//        ypos += lineHeight;
//    }
//
//    if (hasBorderColor) {
//        const auto borderColor{ boxStyle->borderColor.value };
//        const auto border{ YGNodeLayoutGetBorderRect(this->ygNode) };
//
//        ctx_set_rgba_u8(ctx, GetR(borderColor), GetG(borderColor), GetB(borderColor), GetA(borderColor));
//        ctx_rectangle(ctx, 0, 0, w, border.top);
//        ctx_fill(ctx);
//        ctx_rectangle(ctx, 0, 0 + h - border.bottom, w, border.bottom);
//        ctx_fill(ctx);
//        ctx_rectangle(ctx, 0, border.top, border.left, h - border.top - border.bottom);
//        ctx_fill(ctx);
//        // TODO: right side rendering is off by one (edge clipping bug?)
//        ctx_rectangle(ctx, w - border.right - 1.f, 0 + border.top, border.right, h - border.top - border.bottom);
//        ctx_fill(ctx);
//    }
//
//    this->QueueComposite();
}

void TextSceneNode::Composite(CompositeContext* composite) {
//    if (this->layer) {
//        const auto boxStyle{ this->GetStyleOrEmpty() };
//        const auto tintColor{
//            boxStyle->borderColor.empty() ?
//                MixAlpha(boxStyle->color.ValueOr(ColorBlack), composite->CurrentOpacity())
//                    : MixAlpha(ColorWhite, composite->CurrentOpacity())
//        };
//        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
//        const auto transform{
//             ComputeTransform(
//                 composite->CurrentMatrix(),
//                 boxStyle->transform,
//                 boxStyle->transformOriginX,
//                 boxStyle->transformOriginY,
//                 rect,
//                 this->scene)
//        };
//
//        composite->renderer->DrawImage(
//            this->layer,
//            rect,
//            transform,
//            tintColor);
//    }
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
        this->QueueTextLayout();
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
                  this->QueueTextLayout();
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

void TextSceneNode::QueueTextLayout() noexcept {
    this->layout.Reset();
    YGNodeMarkDirty(this->ygNode);
    this->RequestStyleLayout();
}

void TextSceneNode::ClearFontFaceResource() {
    if (this->fontFace) {
        this->fontFace->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->fontFace);
        this->fontFace = nullptr;
    }
}

void TextSceneNode::DestroyRecursive() {
    this->ClearFontFaceResource();

    SceneNode::DestroyRecursive();
}

YGSize TextSceneNode::Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    this->layout.Reset();

//    if (!this->text.empty() && this->style && this->font && this->font->IsReady()) {
//        const auto fontSize{ ComputeFontSize(this->style->fontSize, this->scene) };
//        const auto lineHeight{ ComputeLineHeight(this->style->lineHeight, this->scene,
//            this->font->GetFont()->LineHeight(fontSize)) };
//
//        this->layout.Layout(
//            TextLayoutFont(this->font->GetFont().get(), fontSize, lineHeight),
//            this->style->textOverflow,
//            GetMaxLines(this->style),
//            TextTransform(this->Env(), this->style->textTransform, this->text),
//            width,
//            height);
//    }

    return { this->layout.Width(), this->layout.Height() };
}

//static int32_t GetMaxLines(Style* style) noexcept {
//    const auto& maxLines{ style->maxLines };
//
//    if (!maxLines.empty() && maxLines.unit == StyleNumberUnitPoint) {
//        return maxLines.value;
//    }
//
//    return 0;
//}
//
//static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text) {
//    HandleScope scope(env);
//
//    switch (transform) {
//        case StyleTextTransformUppercase:
//            return ToUpperCase(String::New(env, text));
//        case StyleTextTransformLowercase:
//            return ToLowerCase(String::New(env, text));
//        case StyleTextTransformNone:
//        default:
//            return text;
//    }
//}

} // namespace ls
