/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"
#include "FontResource.h"
#include "Scene.h"
#include <fmt/format.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

constexpr YGSize emptySize{ 0.f, 0.f };

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    this->isLeaf = true;

    YGNodeSetContext(this->ygNode, this);

    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto self { static_cast<TextSceneNode*>(YGNodeGetContext(nodeRef)) };

            if (!self) {
                return emptySize;
            }

            auto& textBlock{ self->textBlock };

            textBlock.Layout();

            return { textBlock.GetComputedWidth(), textBlock.GetComputedHeight() };
    });
}

Function TextSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "TextSceneNode", {
            InstanceAccessor("x", &SceneNode::GetX, nullptr),
            InstanceAccessor("y", &SceneNode::GetY, nullptr),
            InstanceAccessor("width", &SceneNode::GetWidth, nullptr),
            InstanceAccessor("height", &SceneNode::GetHeight, nullptr),

            InstanceAccessor("parent", &SceneNode::GetParent, nullptr),
            InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),
            InstanceAccessor("text", &TextSceneNode::GetText, &TextSceneNode::SetText),

            InstanceMethod("destroy", &SceneNode::Destroy),
            InstanceMethod("appendChild", &SceneNode::AppendChild),
            InstanceMethod("removeChild", &SceneNode::RemoveChild),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void TextSceneNode::Paint(Renderer* renderer) {
    this->textBlock.Paint(renderer);
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

    auto selectedFontResource{ this->scene->GetResourceManager()->FindFont(myStyle->fontFamily(), myStyle->fontStyle(),
        myStyle->fontWeight()) };

    if (!this->SetFontResource(selectedFontResource)) {
        if (selectedFontResource) {
            selectedFontResource->RemoveRef();
        }
    } else {
        YGNodeMarkDirty(this->ygNode);
    }

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

bool TextSceneNode::SetFontResource(FontResource* newFontResource) {
    if (newFontResource == this->fontResource) {
        return false;
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
                YGNodeMarkDirty(this->ygNode);
                this->fontResource->RemoveListener(this->fontResourceListenerId);
                this->fontResourceListenerId = 0;
            }
        });
        this->fontResource = newFontResource;
    }

    return true;
}

void TextSceneNode::DestroyRecursive() {
    this->SetFontResource(nullptr);

    SceneNode::DestroyRecursive();
}

} // namespace ls
