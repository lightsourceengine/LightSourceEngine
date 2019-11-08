/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SceneNode.h"
#include "Style.h"
#include "Scene.h"
#include "yoga-ext.h"
#include "CompositeContext.h"
#include <ls/Renderer.h>
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
    YGNodeSetContext(this->ygNode, this);

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
    if (this->style == nullptr) {
        this->style = Style::New();
        this->style->Bind(this);
    }

    return this->style->Value();
}

void SceneNode::SetStyle(const CallbackInfo& info, const Napi::Value& value) {
    if (this->style == nullptr) {
        this->style = Style::New();
        this->style->Bind(this);

        if (value.IsNull() || value.IsUndefined()) {
            return;
        }
    }

    Style* other{};

    if (value.IsNull() || value.IsUndefined()) {
        other = Style::Empty();
    } else if (value.IsObject()) {
        other = Style::Unwrap(value.As<Object>());
    }

    if (other == nullptr) {
        throw Error::New(info.Env(), "style can only be aassigned to a Style class instance");
    }

    this->style->Assign(other);
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

    this->layer = nullptr;

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

void SceneNode::ComputeStyle() {
    for (auto& child : this->children) {
        child->ComputeStyle();
    }
}

void SceneNode::Paint(Renderer* renderer) {
    for (auto& child : this->children) {
        child->Paint(renderer);
    }
}

void SceneNode::Composite(CompositeContext* context, Renderer* renderer) {
    const auto bounds{ YGNodeLayoutGetRect(this->ygNode) };
    const auto clip{ this->GetStyleOrEmpty()->overflow == YGOverflowHidden };

    context->PushMatrix(Matrix::Translate(bounds.x, bounds.y));

    if (clip) {
        context->PushClipRect(bounds);
        renderer->SetClipRect(context->CurrentClipRect());
    }

    for (auto& child : this->children) {
        child->Composite(context, renderer);
    }

    if (clip) {
        context->PopClipRect();
    }

    context->PopMatrix();
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

Style* SceneNode::GetStyleOrEmpty() const noexcept {
    return this->style ? this->style : Style::Empty();
}

bool SceneNode::InitLayerRenderTarget(Renderer* renderer, int32_t width, int32_t height) {
    if (this->layer && this->layer->IsAttached() && this->layer->GetWidth() == width
            && this->layer->GetHeight() == height) {
        return renderer->SetRenderTarget(this->layer);
    }

    this->layer = renderer->CreateRenderTarget(width, height);

    return this->layer && renderer->SetRenderTarget(this->layer);
}

bool SceneNode::InitLayerSoftwareRenderTarget(Renderer* renderer, int32_t width, int32_t height) {
    if (this->layer && this->layer->IsAttached() && this->layer->GetWidth() == width
            && this->layer->GetHeight() == height) {
        return true;
    }

    this->layer = renderer->CreateTexture(width, height);

    return static_cast<bool>(this->layer);
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
