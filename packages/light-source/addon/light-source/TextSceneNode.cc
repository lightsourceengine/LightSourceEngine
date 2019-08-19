/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "Scene.h"
#include "StyleUtils.h"
#include <napi-ext.h>
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

    this->textBlock.Paint(renderer, x, y, myStyle->color()->Get());
}

Value TextSceneNode::GetText(const CallbackInfo& info) {
    return String::New(info.Env(), this->text);
}

void TextSceneNode::SetText(const CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->text = value.As<String>();
        this->textBlock.SetText(this->ApplyTextTransform(this->text));
    } else if (value.IsNull() || value.IsUndefined()) {
        this->textBlock.SetText("");
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
    FontResource* selectedFont;
    int32_t fontSize;

    if (myStyle->HasFont()) {
        selectedFont = this->scene->GetResourceManager()->FindFont(
            myStyle->fontFamily(),
            myStyle->fontStyle(),
            myStyle->fontWeight());
        fontSize = ComputeIntegerPointValue(myStyle->fontSize(), this->scene, defaultFontSize);
    } else {
        selectedFont = nullptr;
        fontSize = defaultFontSize;
    }

    this->SetFontResource(selectedFont);

    if (selectedFont != nullptr && selectedFont->IsReady()) {
        this->textBlock.SetFont(selectedFont->GetFont(fontSize));
    } else {
        this->textBlock.SetFont(nullptr);
    }

    this->textBlock.SetTextOverflow(myStyle->textOverflow());
    this->textBlock.SetTextAlign(myStyle->textAlign());

    auto newTextTransform{ myStyle->textTransform() };

    if (this->textTransform != newTextTransform) {
        this->textTransform = newTextTransform;
        this->textBlock.SetText(this->ApplyTextTransform(this->text));
    }

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

void TextSceneNode::SetFontResource(FontResource* newFontResource) {
    if (newFontResource == this->fontResource) {
        if (newFontResource) {
            newFontResource->RemoveRef();
        }

        return;
    }

    if (this->fontResource) {
        this->fontResource->RemoveListener(this->fontResourceListenerId);
        this->fontResource->RemoveRef();
        this->fontResource = nullptr;
        this->fontResourceListenerId = 0;
    }

    if (newFontResource) {
        this->fontResourceListenerId = newFontResource->AddListener([this]() {
            if (this->fontResource->IsReady() || this->fontResource->HasError()) {
                if (this->fontResource->IsReady()) {
                    int32_t fontSize{ ComputeIntegerPointValue(
                        this->GetStyleOrEmpty()->fontSize(), this->scene, defaultFontSize) };

                    this->textBlock.SetFont(this->fontResource->GetFont(fontSize));
                } else {
                    this->textBlock.SetFont(nullptr);
                }

                this->fontResource->RemoveListener(this->fontResourceListenerId);
                this->fontResourceListenerId = 0;
                YGNodeMarkDirty(this->ygNode);
            }
        });

        this->fontResource = newFontResource;
    }
}

void TextSceneNode::DestroyRecursive() {
    this->SetFontResource(nullptr);
    this->textBlock.SetFont(nullptr);

    SceneNode::DestroyRecursive();
}

void TextSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on text nodes");
}

std::string TextSceneNode::ApplyTextTransform(const std::string& text) {
    return this->ApplyTextTransform(String::New(this->Env(), text));
}

std::string TextSceneNode::ApplyTextTransform(Napi::String text) {
    switch (this->textTransform) {
        case StyleTextTransformUppercase:
            return ToUpperCase(this->Env(), text);
        case StyleTextTransformLowercase:
            return ToLowerCase(this->Env(), text);
        case StyleTextTransformNone:
            return text;
    }
}

} // namespace ls
