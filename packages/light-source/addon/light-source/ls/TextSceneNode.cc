/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "Stage.h"
#include "Scene.h"
#include "Font.h"
#include "FontStore.h"
#include "StyleUtils.h"
#include "yoga-ext.h"
#include "Style.h"
#include <ls/CompositeContext.h>
#include <ls/Timer.h>
#include <ls/Surface.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>
#include <ls-ctx.h>
#include <napi-ext.h>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

static int32_t GetMaxLines(Style* style) noexcept;
static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text);

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto self{ static_cast<TextSceneNode*>(YGNodeGetContext(nodeRef)) };

            if (!self) {
                return { 0.f, 0.f };
            }

            try {
                return self->Measure(width, widthMode, height, heightMode);
            } catch (const std::exception& e) {
                LOG_ERROR("text measure: %s", e);
            }

            return { 0.f, 0.f };
    });
}

Function TextSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(
            env,
            "TextSceneNode",
            SceneNode::Extend<TextSceneNode>(env, {
                InstanceAccessor("text", &TextSceneNode::GetText, &TextSceneNode::SetText),
            }));

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void TextSceneNode::OnPropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::fontFamily:
        case StyleProperty::fontSize:
        case StyleProperty::fontStyle:
        case StyleProperty::fontWeight:
            this->QueueBeforeLayout();
            break;
        case StyleProperty::lineHeight:
        case StyleProperty::maxLines:
        case StyleProperty::textOverflow:
        case StyleProperty::textTransform:
            YGNodeMarkDirty(this->ygNode);
            this->QueuePaint();
            break;
        case StyleProperty::textAlign:
        case StyleProperty::borderColor:
            this->QueuePaint();
            break;
        case StyleProperty::color:
            this->QueuePaint();
            break;
        case StyleProperty::opacity:
            this->QueueComposite();
            break;
        case StyleProperty::overflow:
            break;
        default:
            if (IsYogaLayoutProperty(property)) {
                this->QueueAfterLayout();
            }
            break;
    }

    SceneNode::OnPropertyChanged(property);
}

void TextSceneNode::BeforeLayout() {
    if (this->SetFont(this->style)) {
        YGNodeMarkDirty(this->ygNode);
        this->QueueAfterLayout();
        this->QueuePaint();
    }
}

void TextSceneNode::AfterLayout() {
    if (YGNodeGetHasNewLayout(this->ygNode)) {
        YGNodeSetHasNewLayout(this->ygNode, false);

        // TODO: Layout change may not need a repaint. If only the position changes, only a composite should be queued.
        this->QueuePaint();
    }
}

void TextSceneNode::Paint(Renderer* renderer) {
    if (!this->font || !this->font->IsReady() || this->layout.IsEmpty() || !this->style || this->style->color.empty()) {
        return;
    }

    const auto color{ this->style->color.value };
    const auto inner{ YGNodeLayoutGetInnerRect(this->ygNode) };
    const auto box{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
    const auto w{ box.width };
    const auto h{ box.height };

    // TODO: if layout dimensions are animated, this will create and recreate the texture every frame!
    // TODO: if no border, color can be a composite field and the layer can be sized to the text layout dimensions,
    //       rather than the (probably) much larger box dimensions.
    if (!this->InitLayerSoftwareRenderTarget(renderer, static_cast<int32_t>(w), static_cast<int32_t>(h))) {
        return;
    }

    const auto surface{this->layer->Lock()};

    // TODO: do not allocate on every repaint!
    Ctx* ctx = ctx_new_for_framebuffer(
        surface.Pixels(), surface.Width(), surface.Height(), surface.Pitch(),
        CTX_FORMAT_RGBA8);

    surface.FillTransparent();

    const auto fontSize{ ComputeFontSize(this->style->fontSize, this->scene) };
    const auto lineHeight{ ComputeLineHeight(this->style->lineHeight, this->scene,
        this->font->GetFont()->LineHeight(fontSize)) };
    const auto textAlign{ this->style->textAlign };
    auto xpos{ 0.f };
    auto ypos{inner.y + this->font->GetFont()->Ascent(fontSize) };

    ctx_set_font(ctx, this->font->GetId().c_str());
    ctx_set_font_size(ctx, fontSize);
    ctx_set_rgba_u8(ctx, GetR(color), GetG(color), GetB(color), GetA(color));

    for (auto& textLine : this->layout.lines) {
        switch (textAlign) {
            case StyleTextAlignLeft:
                xpos = inner.x;
                break;
            case StyleTextAlignCenter:
                xpos = inner.x + ((inner.width - textLine.width) / 2.f);
                break;
            case StyleTextAlignRight:
                xpos = inner.x + inner.width - textLine.width;
                break;
        }

        ctx_move_to(ctx, xpos, ypos);
        ctx_text_w(ctx, textLine.chars.data(), textLine.chars.size());

        ypos += lineHeight;
    }

    if (!this->style->borderColor.empty()) {
        const auto borderColor{ this->style->borderColor.value };
        const auto border{ YGNodeLayoutGetBorderRect(this->ygNode) };

        ctx_set_rgba_u8(ctx, GetR(borderColor), GetG(borderColor), GetB(borderColor), GetA(borderColor));
        ctx_set_line_width(ctx, border.top);
        ctx_rectangle(ctx, 0, 0, w, border.top);
        ctx_fill(ctx);
        ctx_set_line_width(ctx, border.bottom);
        ctx_rectangle(ctx, 0, 0 + h - border.bottom, w, border.bottom);
        ctx_fill(ctx);
        ctx_set_line_width(ctx, border.left);
        ctx_rectangle(ctx, 0, border.top, border.left, h - border.top - border.bottom);
        ctx_fill(ctx);
        ctx_set_line_width(ctx, border.right);
        // TODO: right side rendering is off by one (edge clipping bug?)
        ctx_rectangle(ctx, w - border.right - 1.f, 0 + border.top, border.right, h - border.top - border.bottom);
        ctx_fill(ctx);
    }

    ctx_free(ctx);

    this->QueueComposite();
}

void TextSceneNode::Composite(CompositeContext* context) {
    if (!this->layer) {
        return;
    }

    const auto boxStyle{ this->GetStyleOrEmpty() };
    auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    rect.width = this->layer->GetWidth();
    rect.height = this->layer->GetHeight();

    context->renderer->DrawImage(
        this->layer,
        rect,
        context->CurrentMatrix() * boxStyle->transform.ToMatrix(rect.width, rect.height),
        RGB(255, 255, 255));

    SceneNode::Composite(context);
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
        YGNodeMarkDirty(this->ygNode);
    }
}

bool TextSceneNode::SetFont(Style* style) {
    auto dirty{ false };

    if (!style || style->fontFamily.empty() || style->fontSize.empty()) {
        dirty = !!this->font;
        this->ClearFont();

        return dirty;
    }

    if (this->font && this->font->IsSame(style->fontFamily, style->fontStyle, style->fontWeight)) {
        return false;
    }

    this->ClearFont();

    this->font = this->scene->GetStage()->GetFontStore()->FindFont(
        style->fontFamily, style->fontStyle, style->fontWeight);

    if (!this->font) {
        return true;
    }

    switch (this->font->GetState()) {
        case ResourceStateReady:
            dirty = true;
            break;
        case ResourceStateError:
            this->font = nullptr;
            dirty = true;
            break;
        default:
            this->font.Listen([this, ptr = this->font.Get()]() {
                if (this->font != ptr) {
                    return;
                }

                this->font.Unlisten();
                YGNodeMarkDirty(this->ygNode);
            });
            break;
    }

    return dirty;
}

void TextSceneNode::ClearFont() noexcept {
    this->font = nullptr;
}

void TextSceneNode::DestroyRecursive() {
    this->ClearFont();

    SceneNode::DestroyRecursive();
}

void TextSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on text nodes");
}

YGSize TextSceneNode::Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    this->layout.Layout();

    if (this->style && this->font && this->font->IsReady()) {
        const auto fontSize{ ComputeFontSize(this->style->fontSize, this->scene) };
        const auto lineHeight{ ComputeLineHeight(this->style->lineHeight, this->scene,
            this->font->GetFont()->LineHeight(fontSize)) };

        this->layout.Layout(
            TextLayoutFont(this->font->GetFont().get(), fontSize, lineHeight),
            this->style->textOverflow,
            GetMaxLines(this->style),
            TextTransform(this->Env(), this->style->textTransform, this->text),
            width,
            height);
    }

    return { this->layout.Width(), this->layout.Height() };
}

static int32_t GetMaxLines(Style* style) noexcept {
    const auto& maxLines{ style->maxLines };

    if (!maxLines.empty() && maxLines.unit == StyleNumberUnitPoint) {
        return maxLines.value;
    }

    return 0;
}

static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text) {
    HandleScope scope(env);

    switch (transform) {
        case StyleTextTransformUppercase:
            return ToUpperCase(String::New(env, text));
        case StyleTextTransformLowercase:
            return ToLowerCase(String::New(env, text));
        case StyleTextTransformNone:
        default:
            return text;
    }
}

} // namespace ls
