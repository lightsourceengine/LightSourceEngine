/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "Scene.h"
#include "StyleUtils.h"
#include "LayerResource.h"
#include "Timer.h"
#include <Utils.h>
#include <napi-ext.h>
#include <utf8.h>
#include <fmt/format.h>

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

constexpr int32_t UnicodeNewLine{ 0x0A };
int32_t GetMaxLines(Style* style);
bool LineHeightsEqual(Style* a, Style* b);
std::string TextTransform(Napi::Env env, StyleTextTransform transform, const std::string& text);
bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines);

constexpr int32_t defaultFontSize{ 16 };

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    YGNodeSetContext(this->ygNode, this);

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

void TextSceneNode::Paint(Renderer* renderer) {
    auto myStyle{ this->GetStyleOrEmpty() };

    if (!myStyle->color()) {
        return;
    }

    auto x{ YGNodeLayoutGetLeft(this->ygNode) };
    auto y{ YGNodeLayoutGetTop(this->ygNode) };

    if (!this->layer) {
        this->layer = this->scene->GetResourceManager()->CreateLayerResource();
    }

    if (this->layer->HasError()) {
        return;
    }

    // TODO: layout size may have changed (layout should invalidate layer)
    if (!this->layer->IsReady() && !this->textLines.empty()) {
        fmt::println("layer size {}x{}", YGNodeLayoutGetWidth(this->ygNode), YGNodeLayoutGetHeight(this->ygNode));

        // TODO: layout dimension could be 0

        this->layer->Sync(
            std::min(static_cast<int32_t>(YGNodeLayoutGetWidth(this->ygNode)), this->scene->GetWidth()),
            std::min(static_cast<int32_t>(YGNodeLayoutGetHeight(this->ygNode)), this->scene->GetHeight()));

        auto lockTextureInfo{ renderer->LockTexture(this->layer->TextureId()) };

        Surface surface(
            lockTextureInfo.pixels,
            lockTextureInfo.width,
            lockTextureInfo.height,
            lockTextureInfo.pitch,
            lockTextureInfo.format);

        surface.FillTransparent();

        auto y{ 0.f };

        for (auto& textLine : this->textLines) {
            textLine.Paint(0, y, surface);
            y += this->font->GetLineHeight();
        }
    }

    if (this->layer->IsReady()) {
        renderer->DrawImage(
            this->layer->TextureId(),
            { x, y, static_cast<float>(this->layer->Width()), static_cast<float>(this->layer->Height()) },
            myStyle->color()->Get());
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
        YGNodeMarkDirty(this->ygNode);
    }
}

bool TextSceneNode::SetFont(Style* style) {
    auto dirty{ false };

    if (!style || !style->HasFont()) {
        if (this->font) {
            dirty = true;
        }

        this->ClearFont();

        return dirty;
    }

    auto fontSize{ ComputeIntegerPointValue(style->fontSize(), this->scene, defaultFontSize) };

    auto fontResourceListener = [this, fontSize]() {
        this->fontResource->RemoveListener(this->fontResourceListenerId);
        this->fontResourceListenerId = 0;

        if (this->fontResource->IsReady()) {
            this->font = this->fontResource->GetFont(fontSize);
        } else if (this->fontResource->HasError()) {
            this->font = nullptr;
        }

        YGNodeMarkDirty(this->ygNode);
    };

    if (this->fontResource && this->fontResource->GetFontFamily() == style->fontFamily()
            && this->fontResource->GetFontStyle() == style->fontStyle()
            && this->fontResource->GetFontWeight() == style->fontWeight()) {
        if (this->fontResourceListenerId) {
            this->fontResource->RemoveListener(this->fontResourceListenerId);
            this->fontResourceListenerId = this->fontResource->AddListener(fontResourceListener);
        } else if (this->fontResource->IsReady()) {
            if (this->font->GetSize() != fontSize) {
                this->font = this->fontResource->GetFont(fontSize);
                dirty = true;
            }
        }

        return dirty;
    }

    this->ClearFont();

    auto newFontResource{ this->scene->GetResourceManager()->FindFont(
        style->fontFamily(),
        style->fontStyle(),
        style->fontWeight()) };

    if (newFontResource) {
        this->fontResource = newFontResource;

        if (newFontResource->IsReady()) {
            this->font = newFontResource->GetFont(fontSize);
            dirty = true;
        } else if (newFontResource->HasError()) {
            this->ClearFont();
            dirty = true;
        } else {
            this->fontResourceListenerId = this->fontResource->AddListener(fontResourceListener);
        }
    } else {
        this->font = nullptr;
        dirty = true;
    }

    return dirty;
}

void TextSceneNode::ApplyStyle(Style* newStyle, Style* oldStyle) {
    SceneNode::ApplyStyle(newStyle, oldStyle);

    // TODO: keep line height and max lines and font size (rem, viewport calc)

    if (newStyle == oldStyle) {
        return;
    }

    auto fontChanged{ SetFont(newStyle) };
    auto current{ oldStyle ? oldStyle : Style::Empty() };
    auto style{ newStyle ? newStyle : Style::Empty() };

    if (fontChanged
            || current->textOverflow() != style->textOverflow()
            || current->textAlign() != style->textAlign()
            || current->textTransform() != style->textTransform()
            || GetMaxLines(current) != GetMaxLines(style)
            || !LineHeightsEqual(current, style)) {
        YGNodeMarkDirty(this->ygNode);
    }
}

void TextSceneNode::ClearFont() {
    if (this->fontResource) {
        if (this->fontResourceListenerId) {
            this->fontResource->RemoveListener(this->fontResourceListenerId);
            this->fontResourceListenerId = 0;
        }

        this->fontResource = nullptr;
        this->font = nullptr;
    }
}

void TextSceneNode::DestroyRecursive() {
    this->ClearFont();
    this->scene->GetResourceManager()->RemoveLayerResource(this->layer);

    SceneNode::DestroyRecursive();
}

void TextSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on text nodes");
}

YGSize TextSceneNode::Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    this->textLines.clear();
    this->computedTextWidth = this->computedTextHeight = 0.f;

    if (!this->font || !this->style || this->text.empty()) {
        return { 0.f, 0.f };
    }

    auto transformedText{ TextTransform(this->Env(), this->style->textTransform(), this->text) };
    auto textIter{ transformedText.begin() };
    auto lineHeight{ font->GetLineHeight() };
    int32_t codepoint;
    bool appendResult;
    bool hardLineBreak;
    auto maxLines{ GetMaxLines(this->GetStyleOrEmpty()) };

    this->textLines.emplace_back(TextLine(this->font));

    while (textIter != transformedText.end()) {
        codepoint = utf8::unchecked::next(textIter);

        if (codepoint == UnicodeNewLine) {
            appendResult = false;
            hardLineBreak = true;
        } else {
            appendResult = this->textLines.back().Append(codepoint, width);
            hardLineBreak = false;
        }

        if (!appendResult) {
            auto lineNumber{ static_cast<int32_t>(this->textLines.size()) };

            if (!CanAdvanceLine(lineNumber, lineHeight, height, maxLines)) {
                if (width > 0 && this->GetStyleOrEmpty()->textOverflow() == StyleTextOverflowEllipsis) {
                    this->textLines.back().Ellipsize(width);
                }

                break;
            }

            this->textLines.emplace_back(this->textLines.back().Break(hardLineBreak));
        }
    }

    this->textLines.back().Finalize();

    while (!this->textLines.empty()) {
        if (!this->textLines.back().IsEmpty()) {
            break;
        }

        this->textLines.pop_back();
    }

    this->computedTextWidth = 0.f;

    for (auto& textLine : this->textLines) {
        this->computedTextWidth = std::max(this->computedTextWidth, textLine.Width());
    }

    this->computedTextWidth = std::ceil(this->computedTextWidth);
    this->computedTextHeight = std::ceil(this->textLines.size() * lineHeight);

    return { this->computedTextWidth, this->computedTextHeight };
}

bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines) {
    return (maxLines == 0 || currentLineNumber < maxLines)
        && (heightLimit == 0 || ((currentLineNumber + 1) * lineHeight) < heightLimit);
}

int32_t GetMaxLines(Style* style) {
    auto maxLines{ style->maxLines() };

    if (maxLines && maxLines->GetUnit() == StyleNumberUnitPoint) {
        return maxLines->Int32Value();
    }

    return 0;
}

bool LineHeightsEqual(Style* a, Style* b) {
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

std::string TextTransform(Napi::Env env, StyleTextTransform transform, const std::string& text) {
    HandleScope scope(env);

    switch (transform) {
        case StyleTextTransformUppercase:
            return ToUpperCase(env, String::New(env, text));
        case StyleTextTransformLowercase:
            return ToLowerCase(env, String::New(env, text));
        case StyleTextTransformNone:
        default:
            return text;
    }
}

} // namespace ls
