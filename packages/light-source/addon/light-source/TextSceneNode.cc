/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "ResourceManager.h"
#include "Stage.h"
#include "Scene.h"
#include "Font.h"
#include "FontStore.h"
#include "Surface.h"
#include "StyleUtils.h"
#include "LayerResource.h"
#include "Timer.h"
#include <Utils.h>
#include <napi-ext.h>
#include <fmt/println.h>

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

int32_t GetMaxLines(Style* style);
bool LineHeightsEqual(Style* a, Style* b);
std::string TextTransform(Napi::Env env, StyleTextTransform transform, const std::string& text);

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
    auto width{ std::min(YGNodeLayoutGetWidth(this->ygNode), static_cast<float>(this->scene->GetWidth())) };
    auto height{ std::min(YGNodeLayoutGetHeight(this->ygNode), static_cast<float>(this->scene->GetHeight())) };

    if (!this->font || width <= 0 || height <= 0 || this->textBlock.IsEmpty() || !myStyle->color()) {
        return;
    }

    if (!this->layer) {
        this->layer = this->scene->GetResourceManager()->CreateLayerResource();
    }

    if (this->layer->HasError()) {
        return;
    }

    if (this->layer->IsDirty()) {
        fmt::println("layer size {}x{}", width, height);

        Timer t{ "text paint" };

        this->layer->Sync(static_cast<int32_t>(width), static_cast<int32_t>(height));

        t.Log();

        auto lockTextureInfo{ renderer->LockTexture(this->layer->TextureId()) };

        Surface surface(
            lockTextureInfo.pixels,
            lockTextureInfo.width,
            lockTextureInfo.height,
            lockTextureInfo.pitch,
            lockTextureInfo.format);

        surface.FillTransparent();

        t.Log();

        this->textBlock.Paint(
            surface,
            CalculateLineHeight(this->style->lineHeight(), this->scene, this->font->GetLineHeight()),
            this->style->textAlign(),
            width);
    }

    if (this->layer->IsReady()) {
        renderer->DrawImage(
            this->layer->TextureId(),
            { YGNodeLayoutGetLeft(this->ygNode), YGNodeLayoutGetTop(this->ygNode), width, height },
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

    auto fontResourceListener = [this, fontSize](BaseResource<FontId>* resource) {
        if (this->fontResource->IsReady()) {
            this->font = this->fontResource->GetFont(fontSize);
        } else if (this->fontResource->HasError()) {
            this->font = nullptr;
        } else {
            return;
        }

        this->fontResource->RemoveListener(this->fontResourceListenerId);
        this->fontResourceListenerId = 0;
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

    auto newFontResource{ this->scene->GetStage()->GetFontStore()->Find(
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

void MarkDirtyIfViewportSizeDependent(YGNode* ygNode, const StyleNumberValue* value) {
    if (value && value->IsViewportSizeDependent()) {
        YGNodeMarkDirty(ygNode);
    }
}

void MarkDirtyIfRootFontSizeDependent(YGNode* ygNode, const StyleNumberValue* value) {
    if (value && value->IsRootFontSizeDependent()) {
        YGNodeMarkDirty(ygNode);
    }
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
    auto current{ oldStyle ? oldStyle : Style::Empty() };
    auto style{ newStyle ? newStyle : Style::Empty() };

    if (fontChanged
            || current->textOverflow() != style->textOverflow()
            || current->textTransform() != style->textTransform()
            || GetMaxLines(current) != GetMaxLines(style)
            || !LineHeightsEqual(current, style)) {
        YGNodeMarkDirty(this->ygNode);
    }

    if (current->textAlign() != style->textAlign() && this->layer) {
        this->layer->MarkDirty();
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

    if (this->layer) {
        this->layer->MarkDirty();
    }

    return { this->textBlock.GetComputedWidth(), this->textBlock.GetComputedHeight() };
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
