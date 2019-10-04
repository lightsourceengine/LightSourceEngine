/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SceneNode.h"
#include "Style.h"
#include "Scene.h"
#include <fmt/format.h>
#include <algorithm>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::Value;

namespace ls {

int32_t SceneNode::instanceCount{0};

SceneNode::SceneNode(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (info[0].IsObject()) {
        this->scene = ObjectWrap<Scene>::Unwrap(info[0].As<Object>());
    }

    if (!this->scene) {
        throw Error::New(env, "SceneNode constructor expects a Scene object.");
    }

    this->scene->Ref();
    this->ygNode = YGNodeNew();

    instanceCount++;
}

Value SceneNode::GetInstanceCount(const CallbackInfo& info) {
    return Number::New(info.Env(), instanceCount);
}

Value SceneNode::GetX(const CallbackInfo& info) {
    return Number::New(info.Env(), YGNodeLayoutGetLeft(this->ygNode));
}

Value SceneNode::GetY(const CallbackInfo& info) {
    return Number::New(info.Env(), YGNodeLayoutGetTop(this->ygNode));
}

Value SceneNode::GetWidth(const CallbackInfo& info) {
    return Number::New(info.Env(), YGNodeLayoutGetWidth(this->ygNode));
}

Value SceneNode::GetHeight(const CallbackInfo& info) {
    return Number::New(info.Env(), YGNodeLayoutGetHeight(this->ygNode));
}

Value SceneNode::GetParent(const CallbackInfo& info) {
    if (this->parent == nullptr) {
        return info.Env().Null();
    }

    return this->parent->AsReference()->Value();
}

Value SceneNode::GetScene(const CallbackInfo& info) {
    if (this->scene == nullptr) {
        return info.Env().Null();
    }

    return this->scene->Value();
}

Napi::Value SceneNode::GetChildren(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto childArray{ Array::New(env, this->children.size() )};
    auto i{ 0u };

    for (auto& child : this->children) {
        childArray[i++] = child->AsReference()->Value();
    }

    return scope.Escape(childArray);
}

Value SceneNode::GetStyle(const CallbackInfo& info) {
    if (this->style) {
        return this->style->Value();
    }

    return info.Env().Null();
}

void SceneNode::SetStyle(const CallbackInfo& info, const Napi::Value& value) {
    HandleScope scope(info.Env());
    Style* newStyle;
    auto oldStyle{ this->style };

    if (!value.IsObject()) {
        newStyle = nullptr;
    } else {
        newStyle = ObjectWrap<Style>::Unwrap(value.As<Object>());
    }

    if (newStyle == oldStyle) {
        return;
    }

    if (newStyle) {
        newStyle->Ref();
    }

    this->style = newStyle;

    this->UpdateStyle(newStyle, oldStyle);

    if (oldStyle) {
        oldStyle->Unref();
    }
}

void SceneNode::SetParent(SceneNode* newParent) {
    if (newParent == this->parent) {
        return;
    }

    if (this->parent) {
        this->parent->AsReference()->Unref();
        this->parent = nullptr;
    }

    if (newParent) {
        this->parent = newParent;
        this->parent->AsReference()->Ref();
    }
}

void SceneNode::OnViewportSizeChange() {
    if (this->style) {
        this->style->ApplyViewportSize(this->ygNode, this->scene->GetWidth(), this->scene->GetHeight());
    }

    for (auto child : this->children) {
        child->OnRootFontSizeChange();
    }
}

void SceneNode::OnRootFontSizeChange() {
    if (this->style) {
        this->style->ApplyRootFontSize(this->ygNode, this->scene->GetRootFontSize());
    }

    for (auto child : this->children) {
        child->OnRootFontSizeChange();
    }
}

void SceneNode::UpdateStyle(Style* newStyle, Style* oldStyle) {
    if (newStyle == nullptr) {
        newStyle = Style::Empty();
    }

    newStyle->Reset(
        this->ygNode,
        this->scene->GetWidth(),
        this->scene->GetHeight(),
        this->scene->GetRootFontSize());
}

void SceneNode::AppendChild(const CallbackInfo& info) {
    auto childObject{ info[0].As<Object>() };
    auto child{ ObjectWrap<SceneNode>::Unwrap(childObject) };

    this->ValidateInsertCandidate(child);

    this->AppendChild(child);
}

void SceneNode::InsertBefore(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto childObject{ info[0].As<Object>() };
    auto child{ ObjectWrap<SceneNode>::Unwrap(childObject) };

    this->ValidateInsertCandidate(child);

    auto beforeObject{ info[1].As<Object>() };
    auto before{ ObjectWrap<SceneNode>::Unwrap(beforeObject) };

    if (before == nullptr || before->parent != this) {
        throw Error::New(env, "before must be a child of this SceneNode");
    }

    this->InsertBefore(child, before);
}

void SceneNode::RemoveChild(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto childObject{ info[0].As<Object>() };
    auto child{ ObjectWrap<SceneNode>::Unwrap(childObject) };

    if (child == nullptr) {
        throw Error::New(env, "Node to remove must be a SceneNode instance.");
    }

    if (child->parent != this) {
        throw Error::New(env, "Node to remove is not a child of this SceneNode.");
    }

    this->RemoveChild(child);
}

void SceneNode::AppendChild(SceneNode* child) {
    this->children.push_back(child);
    child->AsReference()->Ref();
    child->SetParent(this);

    YGNodeInsertChild(this->ygNode, child->ygNode, YGNodeGetChildCount(this->ygNode));
}

void SceneNode::InsertBefore(SceneNode* child, SceneNode* before) {
    auto beforeIndex{ -1 };
    auto childrenLen{ static_cast<int32_t>(this->children.size()) };

    for (auto i{ 0 }; i < childrenLen; i++) {
        if (this->children[i] == before) {
            beforeIndex = i;
            break;
        }
    }

    auto reference{ child->AsReference() };

    if (beforeIndex < 0) {
        throw Error::New(reference->Env(), "before is not a child of SceneNode.");
    }

    this->children.insert(this->children.begin() + beforeIndex, child);

    reference->Ref();
    child->SetParent(this);

    YGNodeInsertChild(this->ygNode, child->ygNode, beforeIndex);
}

void SceneNode::RemoveChild(SceneNode* child) {
    if (!child || child->parent != this) {
        return;
    }

    YGNodeRemoveChild(this->ygNode, child->ygNode);
    this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
    child->AsReference()->Unref();
    child->SetParent(nullptr);
}

void SceneNode::Destroy(const Napi::CallbackInfo& info) {
    this->Destroy();
}

void SceneNode::Destroy() {
    if (this->parent) {
        this->parent->RemoveChild(this);
    }

    this->DestroyRecursive();
}

void SceneNode::DestroyRecursive() {
    if (this->scene == nullptr) {
        return;
    }

    instanceCount--;

    for (auto child : this->children) {
        child->DestroyRecursive();
    }

    this->children.clear();
    this->SetParent(nullptr);

    if (this->ygNode) {
        YGNodeFree(this->ygNode);
        this->ygNode = nullptr;
    }

    if (this->scene) {
        this->scene->Unref();
        this->scene = nullptr;
    }

    if (this->style) {
        this->style->Unref();
        this->style = nullptr;
    }
}

void SceneNode::Paint(Renderer* renderer) {
    if (!this->children.empty()) {
        auto x{ YGNodeLayoutGetLeft(this->ygNode) };
        auto y{ YGNodeLayoutGetTop(this->ygNode) };
        auto clip{ this->GetStyleOrEmpty()->overflow() == YGOverflowHidden };

        if (clip) {
            renderer->PushClipRect({ x, y, YGNodeLayoutGetWidth(this->ygNode), YGNodeLayoutGetHeight(this->ygNode) });
        }

        renderer->Shift(x, y);

        for (auto& node : this->children) {
            node->Paint(renderer);
        }

        renderer->Unshift();

        if (clip) {
            renderer->PopClipRect();
        }
    }
}

void SceneNode::Layout(float width, float height) {
    if (YGNodeIsDirty(this->ygNode)) {
        YGNodeCalculateLayout(this->ygNode, width, height, YGDirectionLTR);
    }
}

void SceneNode::ValidateInsertCandidate(SceneNode* child) {
    auto env{ this->AsReference()->Env() };

    if (child == nullptr) {
        throw Error::New(env, "child must be a SceneNode instance.");
    }

    if (child == this) {
        throw Error::New(env, "child cannot equal this");
    }

    if (child->parent != nullptr) {
        throw Error::New(env, "child already has a parent.");
    }
}

void SceneNode::Focus(const CallbackInfo& info) {
    HandleScope scope(info.Env());

    this->scene->SetActiveNode(this->AsReference()->Value());
}

void SceneNode::Blur(const CallbackInfo& info) {
    HandleScope scope(info.Env());

    this->scene->SetActiveNode(info.Env().Null());
}

} // namespace ls
