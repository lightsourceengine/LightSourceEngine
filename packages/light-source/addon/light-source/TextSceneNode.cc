/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "FontSampleResource.h"
#include "Scene.h"
#include "StyleUtils.h"
#include <fmt/format.h>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    YGNodeSetContext(this->ygNode, this);

    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto self { static_cast<TextSceneNode*>(YGNodeGetContext(nodeRef)) };

            if (!self) {
                return { 0.f, 0.f };
            }

            auto& textBlock{ self->textBlock };

            if (textBlock.IsDirty()) {
                textBlock.Layout(width, widthMode, height, heightMode);
            }

            return { textBlock.GetComputedWidth(), textBlock.GetComputedHeight() };
    });
}

Function TextSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(
            env,
            "TextSceneNode",
            SceneNode::Extend<TextSceneNode>({
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

    this->textBlock.Paint(renderer, x, y, myStyle->color()->Get());
}

Value TextSceneNode::GetText(const CallbackInfo& info) {
    return String::New(info.Env(), this->textBlock.GetText());
}

void TextSceneNode::SetText(const CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->textBlock.SetText(value.As<String>());
    } else if (value.IsNull() || value.IsUndefined()) {
        this->textBlock.SetText(std::string());
    } else {
        throw Error::New(info.Env(), "Cannot assign non-string value to text property.");
    }

    if (this->textBlock.IsDirty()) {
        YGNodeMarkDirty(this->ygNode);
    }
}

void TextSceneNode::ApplyStyle(Style* style) {
    SceneNode::ApplyStyle(style);

    auto myStyle{ this->GetStyleOrEmpty() };
    FontSampleResource* selectedFont;

    if (myStyle->HasFont()) {
        selectedFont = this->scene->GetResourceManager()->FindFontSample(
            myStyle->fontFamily(),
            myStyle->fontStyle(),
            myStyle->fontWeight(),
            ComputeIntegerPointValue(myStyle->fontSize(), this->scene, 16));

        if (selectedFont == nullptr) {
            selectedFont = this->scene->GetResourceManager()->LoadFontSample(
                myStyle->fontFamily(),
                myStyle->fontStyle(),
                myStyle->fontWeight(),
                ComputeIntegerPointValue(myStyle->fontSize(), this->scene, 16));
        }
    } else {
        selectedFont = nullptr;
    }

    if (!this->SetFont(selectedFont)) {
        if (selectedFont) {
            selectedFont->RemoveRef();
        }
    } else {
        // TODO: font may not be ready
        YGNodeMarkDirty(this->ygNode);
    }

    this->textBlock.SetFont(selectedFont);
    this->textBlock.SetTextOverflow(myStyle->textOverflow());
    this->textBlock.SetTextAlign(myStyle->textAlign());
    this->textBlock.SetTextTransform(myStyle->textTransform());

    int32_t maxLines;

    if (myStyle->maxLines() && myStyle->maxLines()->GetUnit() == StyleNumberUnitPoint) {
        maxLines = myStyle->maxLines()->Int32Value();
    } else {
        maxLines = 0;
    }

    this->textBlock.SetMaxLines(maxLines);

    if (this->textBlock.IsDirty()) {
        YGNodeMarkDirty(this->ygNode);
    }
}

bool TextSceneNode::SetFont(FontSampleResource* newFont) {
    if (newFont == this->font) {
        return false;
    }

    if (this->font) {
        this->font->RemoveListener(this->fontListenerId);
        this->font->RemoveRef();
        this->font = nullptr;
        this->fontListenerId = 0;
    }

    if (newFont) {
        this->fontListenerId = newFont->AddListener([this]() {
            if (this->font->IsReady() || this->font->HasError()) {
                this->textBlock.SetFont(this->font);
                YGNodeMarkDirty(this->ygNode);
                this->font->RemoveListener(this->fontListenerId);
                this->fontListenerId = 0;
            }
        });

        this->font = newFont;
    }

    return true;
}

void TextSceneNode::DestroyRecursive() {
    this->SetFont(nullptr);

    SceneNode::DestroyRecursive();
}

void TextSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild is an unsupported operation on text nodes");
}

} // namespace ls
