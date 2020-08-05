/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SceneNode.h"
#include "Style.h"
#include "Scene.h"
#include "yoga-ext.h"
#include <ls/CompositeContext.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <algorithm>

#include <ls/BoxSceneNode.h>
#include <ls/RootSceneNode.h>
#include <ls/LinkSceneNode.h>
#include <ls/ImageSceneNode.h>
#include <ls/TextSceneNode.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Value;

namespace ls {

int32_t SceneNode::instanceCount{0};

SceneNode::SceneNode(const CallbackInfo& info) {
}

void SceneNode::BaseConstructor(const Napi::CallbackInfo& info, SceneNodeType type) {
    this->scene = Scene::Cast(info[0]);

    if (!this->scene) {
        throw Error::New(info.Env(), "Expected scene reference as arg.");
    }

    this->ygNode = YGNodeNew();
    instanceCount++;

    try {
        this->scene->Ref();
    } catch (const Error& error) {
        YGNodeFree(this->ygNode);
        this->scene = nullptr;
        this->ygNode = nullptr;
        throw;
    }

    SetType(info.This(), type);
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

    return this->parent->Value();
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
        childArray[i++] = child->Value();
    }

    return scope.Escape(childArray);
}

Value SceneNode::GetStyle(const CallbackInfo& info) {
    if (this->style == nullptr) {
        // New adds a Ref.
        this->style = Style::New(info.Env());
        this->style->Bind(this);
    }

    return this->style->Value();
}

void SceneNode::SetStyle(const CallbackInfo& info, const Napi::Value& value) {
    if (this->style == nullptr) {
        // New adds a Ref.
        this->style = Style::New(info.Env());
        this->style->Bind(this);

        if (value.IsNull() || value.IsUndefined()) {
            return;
        }
    }

    Style* other{};

    if (value.IsNull() || value.IsUndefined()) {
        other = Style::Empty();
    } else if (value.IsObject()) {
        other = Style::Cast(value.As<Object>());
    }

    if (other == nullptr) {
        throw Error::New(info.Env(), "style can only be aassigned to a Style class instance");
    }

    this->style->Assign(other);
}

Napi::Value SceneNode::GetHidden(const CallbackInfo& info) {
    return Boolean::New(info.Env(), this->isHidden);
}

void SceneNode::SetHidden(const CallbackInfo& info, const Napi::Value& value) {
    this->isHidden = value.ToBoolean();
}

void SceneNode::SetParent(SceneNode* newParent) {
    if (newParent == this->parent) {
        return;
    }

    if (this->parent) {
        this->parent->Unref();
        this->parent = nullptr;
    }

    if (newParent) {
        this->parent = newParent;
        this->parent->Ref();
    }
}

Stage* SceneNode::GetStage() const noexcept {
    return this->scene ? this->scene->GetStage() : nullptr;
}

void SceneNode::AppendChild(const CallbackInfo& info) {
    auto child{ SceneNode::QueryInterface(info[0]) };

    this->ValidateInsertCandidate(info.Env(), child);

    this->AppendChild(child);
}

void SceneNode::InsertBefore(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto child{ SceneNode::QueryInterface(info[0]) };

    this->ValidateInsertCandidate(env, child);

    auto before{ SceneNode::QueryInterface(info[1]) };

    if (before == nullptr || before->parent != this) {
        throw Error::New(env, "before must be a child of this SceneNode");
    }

    this->InsertBefore(env, child, before);
}

void SceneNode::RemoveChild(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto childObject{ info[0].As<Object>() };
    auto child{ SceneNode::QueryInterface(childObject) };

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
    this->sortedChildren.clear();
    child->Ref();
    child->SetParent(this);

    YGNodeInsertChild(this->ygNode, child->ygNode, YGNodeGetChildCount(this->ygNode));
}

void SceneNode::InsertBefore(const Napi::Env& env, SceneNode* child, SceneNode* before) {
    auto beforeIndex{ -1 };
    auto childrenLen{ static_cast<int32_t>(this->children.size()) };

    for (auto i{ 0 }; i < childrenLen; i++) {
        if (this->children[i] == before) {
            beforeIndex = i;
            break;
        }
    }

    if (beforeIndex < 0) {
        throw Error::New(env, "before is not a child of SceneNode.");
    }

    this->children.insert(this->children.begin() + beforeIndex, child);
    this->sortedChildren.clear();

    child->Ref();
    child->SetParent(this);

    YGNodeInsertChild(this->ygNode, child->ygNode, beforeIndex);
}

void SceneNode::RemoveChild(SceneNode* child) {
    if (!child || child->parent != this) {
        return;
    }

    YGNodeRemoveChild(this->ygNode, child->ygNode);
    // TODO: remove from scene queues?
    this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
    this->sortedChildren.clear();
    child->Unref();
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

    this->scene->Remove(this);

    for (auto child : this->children) {
        child->DestroyRecursive();
        child->Unref();
    }

    this->children.clear();
    this->sortedChildren.clear();
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
        this->style->Bind(nullptr);
        this->style->Unref();
        this->style = nullptr;
    }
}

void SceneNode::Composite(CompositeContext* context) {
    if (this->isHidden || this->children.empty()) {
        return;
    }

    const auto boxStyle{ this->GetStyleOrEmpty() };
    const auto bounds{ YGNodeLayoutGetRect(this->ygNode) };
    const auto clip{ boxStyle->overflow == YGOverflowHidden };

    context->PushMatrix(Matrix::Translate(bounds.x, bounds.y));
    context->PushOpacity(boxStyle->opacity.AsFloat(1.f));

    if (clip) {
        context->PushClipRect(bounds);
        context->renderer->SetClipRect(context->CurrentClipRect());
    }

    for (auto& child : this->SortChildrenByStackingOrder()) {
        child->Composite(context);
    }

    if (clip) {
        context->PopClipRect();
    }

    context->PopOpacity();
    context->PopMatrix();
}

void SceneNode::OnPropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::transform:
        case StyleProperty::zIndex:
            this->sortedChildren.clear();
            this->QueueComposite();
            break;
        case StyleProperty::opacity:
        case StyleProperty::overflow:
        case StyleProperty::transformOriginX:
        case StyleProperty::transformOriginY:
            this->QueueComposite();
            break;
        default:
            break;
    }
}

void SceneNode::Layout(float width, float height) {
    if (YGNodeIsDirty(this->ygNode)) {
        YGNodeCalculateLayout(this->ygNode, width, height, YGDirectionLTR);

        SceneNode::Visit(this, [](SceneNode* node) {
            node->QueueAfterLayoutIfNecessary();
        });
    }
}

void SceneNode::ValidateInsertCandidate(const Napi::Env& env, SceneNode* child) {
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

void SceneNode::QueuePaint() {
    // TODO: is queued?
    this->scene->QueuePaint(this);
}

void SceneNode::QueueAfterLayoutIfNecessary() {
    if (YGNodeGetHasNewLayout(this->ygNode)) {
        YGNodeSetHasNewLayout(this->ygNode, false);
        this->QueueAfterLayout();
    }
}

void SceneNode::QueueAfterLayout() {
    this->scene->QueueAfterLayout(this);
}

void SceneNode::QueueBeforeLayout() {
    this->scene->QueueBeforeLayout(this);
}

void SceneNode::QueueComposite() {
    this->scene->QueueComposite();
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

    this->scene->SetActiveNode(this->Value());
}

void SceneNode::Blur(const CallbackInfo& info) {
    HandleScope scope(info.Env());

    this->scene->SetActiveNode(info.Env().Null());
}

bool SceneNode::HasTransform() const noexcept {
    return this->style && !this->style->transform.empty();
}

int32_t SceneNode::GetZIndex() const noexcept {
    return this->style ? this->style->zIndex.AsInt32(0) : 0;
}

void SceneNode::MarkDirty() noexcept {
    YGNodeMarkDirty(this->ygNode);
}

const std::vector<SceneNode*>& SceneNode::SortChildrenByStackingOrder() {
    if (!this->sortedChildren.empty()) {
        // already sorted
        return this->sortedChildren;
    }

    this->sortedChildren.reserve(this->children.size());
    std::copy(this->children.begin(), this->children.end(), std::back_inserter(this->sortedChildren));

    // use stable_sort to preserve the original order of nodes when z-indexes are the same
    std::stable_sort(this->sortedChildren.begin(), this->sortedChildren.end(), [](SceneNode* a, SceneNode* b) {
        const auto aHasTransform{ a->HasTransform() };

        if (aHasTransform != b->HasTransform()) {
            // nodes with transform always drawn above nodes with no transform
            //
            // if a has transform and b not has transform, then a is less than b
            // if b has transform and a not has transform, then a is less than b
            return !aHasTransform;
        }

        // by default, nodes have a z-index of 0
        return a->GetZIndex() < b->GetZIndex();
    });

    return this->sortedChildren;
}

SceneNode* SceneNode::QueryInterface(Napi::Value value) {
    if (value.IsObject()) {
        int32_t type = value.As<Object>().Get(0u).As<Number>();

        switch (type) {
            case SceneNodeTypeRoot:
                return RootSceneNode::Cast(value);
            case SceneNodeTypeBox:
                return BoxSceneNode::Cast(value);
            case SceneNodeTypeImage:
                return ImageSceneNode::Cast(value);
            case SceneNodeTypeText:
                return TextSceneNode::Cast(value);
            case SceneNodeTypeLink:
                return LinkSceneNode::Cast(value);
            default:
                break;
        }
    }

    return nullptr;
}

void SceneNode::SetType(Napi::Value value, SceneNodeType type) {
    if (value.IsObject()) {
        value.As<Object>().Set(0u, Number::New(value.Env(), type));
    }
}

} // namespace ls
