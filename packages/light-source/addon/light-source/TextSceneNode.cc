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
#include "Layer.h"
#include "LayerCache.h"
#include "yoga-ext.h"
#include "CompositeContext.h"
#include <ls/Timer.h>
#include <ls/Surface.h>
#include <ls/Renderer.h>
#include <napi-ext.h>
#include <ls/PixelConversion.h>

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
static bool LineHeightsEqual(Style* a, Style* b) noexcept;
static void MarkDirtyIfViewportSizeDependent(YGNode* ygNode, const StyleNumberValue* value) noexcept;
static void MarkDirtyIfRootFontSizeDependent(YGNode* ygNode, const StyleNumberValue* value) noexcept;
static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text);

constexpr int32_t defaultFontSize{ 16 };

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto self{ static_cast<TextSceneNode*>(YGNodeGetContext(nodeRef)) };

            if (!self) {
                return { 0.f, 0.f };
            }

            return self->Measure(width, widthMode, height, heightMode);
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

void TextSceneNode::ComputeStyle() {
}

void TextSceneNode::Paint(Renderer* renderer) {
    auto boxStyle{ this->GetStyleOrEmpty() };

    // TODO: layer should not be a condition
    if (this->layer || !this->font || this->textBlock.IsEmpty() || !boxStyle->color()) {
        return;
    }

    auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerSoftwareRenderTarget(
            renderer,
            static_cast<int32_t>(this->textBlock.GetComputedWidth()),
            static_cast<int32_t>(this->textBlock.GetComputedHeight()))) {
        return;
    }

    Timer t{ "text paint" };

    auto surface{ this->layer->Lock() };

    t.Log();

    if (surface.IsEmpty()) {
        return;
    }

    surface.FillTransparent();

    t.Log();

    this->textBlock.Paint(
        surface,
        CalculateLineHeight(this->style->lineHeight(), this->scene, this->font->GetLineHeight()),
        this->style->textAlign(),
        dest.width);
}

void TextSceneNode::Composite(CompositeContext* context, Renderer* renderer) {
    if (!this->layer) {
        return;
    }

    const auto& transform{ context->CurrentMatrix() };
    auto boxStyle{ this->GetStyleOrEmpty() };
    auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    rect.x += transform.GetTranslateX();
    rect.y += transform.GetTranslateY();
    rect.width = this->layer->GetWidth();
    rect.height = this->layer->GetHeight();

    renderer->DrawImage(this->layer, rect, boxStyle->color() ? boxStyle->color()->Get() : RGB(255, 255, 255));

    SceneNode::Composite(context, renderer);
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

    if (!style || !style->HasFont()) {
        dirty = !!this->fontResource;
        this->ClearFont();

        return dirty;
    }

    const FontId fontId{ style->fontFamily(), style->fontStyle(), style->fontWeight() };

    if (this->fontResource && this->fontResource->GetId() == fontId) {
        return false;
    }

    this->ClearFont();

    this->fontResource = this->scene->GetStage()->GetFontStore()->Get(fontId);

    if (!this->fontResource) {
        return true;
    }

    const auto fontSize{ ComputeIntegerPointValue(style->fontSize(), this->scene, defaultFontSize) };

    switch (this->fontResource->GetState()) {
        case ResourceStateReady:
            this->font = this->fontResource->GetFont(fontSize);
            dirty = true;
            break;
        case ResourceStateError:
            this->fontResource = nullptr;
            dirty = true;
            break;
        default:
            this->fontResource.Listen([this, ptr = this->fontResource.Get(), fontSize]() {
                if (this->fontResource != ptr) {
                    return;
                }

                switch (this->fontResource->GetState()) {
                    case ResourceStateReady:
                        this->font = this->fontResource->GetFont(fontSize);
                        break;
                    case ResourceStateError:
                        this->fontResource = nullptr;
                        break;
                    default:
                        return;
                }

                this->fontResource.Unlisten();
                YGNodeMarkDirty(this->ygNode);
            });
            break;
    }

    return dirty;
}

bool TextSceneNode::SetFontSize(Style* style) {
    if (!style) {
        if (this->font) {
            this->font = nullptr;
            return true;
        } else {
            return false;
        }
    }

    if (!this->fontResource || !this->fontResource->IsReady()) {
        return false;
    }

    const auto fontSize{ ComputeIntegerPointValue(style->fontSize(), this->scene, defaultFontSize) };

    if (this->font && this->font->GetSize() == fontSize) {
        return false;
    }

    this->font = this->fontResource->GetFont(fontSize);

    return true;
}

void TextSceneNode::OnViewportSizeChange() {
    SceneNode::OnViewportSizeChange();

    if (this->style) {
        MarkDirtyIfViewportSizeDependent(this->ygNode, this->style->lineHeight());
        MarkDirtyIfViewportSizeDependent(this->ygNode, this->style->fontSize());
    }
}

void TextSceneNode::OnRootFontSizeChange() {
    SceneNode::OnRootFontSizeChange();

    if (this->style) {
        MarkDirtyIfRootFontSizeDependent(this->ygNode, this->style->lineHeight());
        MarkDirtyIfRootFontSizeDependent(this->ygNode, this->style->fontSize());
    }
}

void TextSceneNode::UpdateStyle(Style* newStyle, Style* oldStyle) {
    SceneNode::UpdateStyle(newStyle, oldStyle);

    if (newStyle == oldStyle) {
        return;
    }

    auto fontChanged{ SetFont(newStyle) };
    auto fontSizeChanged { SetFontSize(newStyle) };
    auto current{ oldStyle ? oldStyle : Style::Empty() };
    auto style{ newStyle ? newStyle : Style::Empty() };

    if (fontChanged
            || fontSizeChanged
            || current->textOverflow() != style->textOverflow()
            || current->textTransform() != style->textTransform()
            || GetMaxLines(current) != GetMaxLines(style)
            || !LineHeightsEqual(current, style)) {
        YGNodeMarkDirty(this->ygNode);
    }

//    if (current->textAlign() != style->textAlign() && this->layer) {
//        this->layer->MarkDirty();
//    }
}

void TextSceneNode::ClearFont() noexcept {
    this->fontResource = nullptr;
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
    this->textBlock.Clear();
    this->textBlock.SetFont(this->font);

    if (this->style && this->font) {
        this->textBlock.Layout(
            TextTransform(this->Env(), this->style->textTransform(), this->text),
            width,
            height,
            this->style->textOverflow() == StyleTextOverflowEllipsis,
            CalculateLineHeight(this->style->lineHeight(), this->scene, this->font->GetLineHeight()),
            GetMaxLines(this->style));
    }

//    if (this->layer) {
//        this->layer->MarkDirty();
//    }

    return { this->textBlock.GetComputedWidth(), this->textBlock.GetComputedHeight() };
}

static void MarkDirtyIfViewportSizeDependent(YGNode* ygNode, const StyleNumberValue* value) noexcept {
    if (value && value->IsViewportSizeDependent()) {
        YGNodeMarkDirty(ygNode);
    }
}

static void MarkDirtyIfRootFontSizeDependent(YGNode* ygNode, const StyleNumberValue* value) noexcept {
    if (value && value->IsRootFontSizeDependent()) {
        YGNodeMarkDirty(ygNode);
    }
}

static int32_t GetMaxLines(Style* style) noexcept {
    auto maxLines{ style->maxLines() };

    if (maxLines && maxLines->GetUnit() == StyleNumberUnitPoint) {
        return maxLines->Int32Value();
    }

    return 0;
}

static bool LineHeightsEqual(Style* a, Style* b) noexcept {
    auto lineHeightA{ a->lineHeight() };
    auto lineHeightB{ b->lineHeight() };

    if (!lineHeightA && !lineHeightB) {
        return true;
    }

    if (!lineHeightA || !lineHeightB) {
        return false;
    }

    return lineHeightA->GetUnit() == lineHeightB->GetUnit()
        && YGFloatsEqual(lineHeightA->GetValue(), lineHeightB->GetValue());
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
