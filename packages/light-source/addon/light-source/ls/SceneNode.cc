/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/SceneNode.h>

#include <cassert>
#include <algorithm>
#include <ls/Style.h>
#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/CompositeContext.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <ls/RootSceneNode.h>
#include <ls/BoxSceneNode.h>
#include <ls/ImageSceneNode.h>
#include <ls/TextSceneNode.h>
#include <ls/LinkSceneNode.h>
#include <ls/Timer.h>
#include <ls/yoga-ext.h>

#include <ls/bindings/JSScene.h>

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

void SceneNode::SceneNodeConstructor(const Napi::CallbackInfo& info) {
    auto jsScene{ bindings::JSScene::Cast(info[0]) };

    NAPI_EXPECT_NOT_NULL(info.Env(), jsScene, "scene arg must be a Scene instance");

    this->scene = jsScene->GetNative();

    this->flags.set(FlagLayoutOnly, true);
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
    auto parent{ this->GetParent() };
    return parent ? parent->Value() : info.Env().Null();
}

Napi::Value SceneNode::GetChildren(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto childArray{ Array::New(env, YGNodeGetChildCount(this->ygNode) )};
    auto i{ 0u };

    for (const auto& child : YGNodeGetChildren(this->ygNode)) {
        childArray[i++] = YGNodeGetContextAs<SceneNode>(child)->Value();
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
    HandleScope scope(info.Env());

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
        throw Error::New(info.Env(), "style can only be assigned to a Style class instance");
    }

    this->style->Assign(other);
}

Napi::Value SceneNode::GetHidden(const CallbackInfo& info) {
    return Boolean::New(info.Env(), this->IsHidden());
}

void SceneNode::SetHidden(const CallbackInfo& info, const Napi::Value& value) {
    this->flags.set(FlagHidden, value.ToBoolean());
}

Resources* SceneNode::GetResources() const noexcept {
    assert(this->scene);
    return this->scene->GetStage()->GetResources();
}

SceneNode* SceneNode::GetParent() const noexcept {
    auto parent{ this->ygNode->getParent() };

    return parent ? YGNodeGetContextAs<SceneNode>(parent) : nullptr;
}

void SceneNode::AppendChild(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (this->IsLeaf()) {
        throw Error::New(env, "appendChild: leaf nodes cannot have children");
    }

    HandleScope scope(env);
    auto child{ SceneNode::QueryInterface(info[0]) };

    if (child == nullptr || child == this) {
        throw Error::New(env, "appendChild: invalid child argument");
    }

    if (child->GetParent() != nullptr) {
        throw Error::New(env, "appendChild: child already has a parent");
    }

    YGNodeInsertChild(this->ygNode, child->ygNode, YGNodeGetChildCount(this->ygNode));
    this->sortedChildren.clear();

    // Add reference for the added child.
    child->Ref();
    // Add reference for the parent.
    this->Ref();
}

void SceneNode::InsertBefore(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (this->IsLeaf()) {
        throw Error::New(env, "insertBefore: leaf nodes cannot have children");
    }

    auto child{ SceneNode::QueryInterface(info[0]) };

    if (child == nullptr || child == this) {
        throw Error::New(env, "insertBefore: invalid child argument");
    }

    if (child->GetParent() != nullptr) {
        throw Error::New(env, "insertBefore: child already has a parent");
    }

    auto before{ SceneNode::QueryInterface(info[1]) };
    auto beforeIndex{ before ? this->GetChildIndex(before) : static_cast<int32_t>(YGNodeGetChildCount(this->ygNode)) };

    if (beforeIndex < 0) {
        throw Error::New(env, "insertBefore: before argument is not a child");
    }

    YGNodeInsertChild(this->ygNode, child->ygNode, beforeIndex);
    this->sortedChildren.clear();

    // Add reference for the added child.
    child->Ref();
    // Add reference for the parent.
    this->Ref();
}

void SceneNode::RemoveChild(const CallbackInfo& info) {
    HandleScope scope(info.Env());

    this->RemoveChild(SceneNode::QueryInterface(info[0]));
}

void SceneNode::RemoveChild(SceneNode* child) noexcept {
    if (GetChildIndex(child) < 0) {
        return;
    }

    YGNodeRemoveChild(this->ygNode, child->ygNode);
    this->sortedChildren.clear();

    // Remove reference for the child.
    child->Unref();

    // Remove reference for the parent
    this->Unref();
}

void SceneNode::Destroy(const Napi::CallbackInfo& info) {
    this->Destroy();
}

void SceneNode::Destroy() {
    if (this->ygNode == nullptr) {
        return;
    }

    instanceCount--;

    this->scene->Remove(this);

    const auto& children{ YGNodeGetChildren(this->ygNode) };

    while (!children.empty()) {
        YGNodeGetContextAs<SceneNode>(children.back())->Destroy();
    }

    auto parent{ this->GetParent() };

    if (parent) {
        parent->RemoveChild(this);
    }

    this->style = Style::RemoveRef(this->style, [](Style* ref) { ref->Bind(nullptr); });
    this->sortedChildren.clear();
    YGNodeFree(this->ygNode);
    this->ygNode = nullptr;
}

void SceneNode::OnStylePropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::transformOriginX:
        case StyleProperty::transformOriginY:
        case StyleProperty::opacity:
            this->RequestComposite();
            break;
        case StyleProperty::transform:
        case StyleProperty::zIndex:
            if (this->GetParent()) {
                this->GetParent()->sortedChildren.clear();
            }
            this->RequestComposite();
            break;
        default:
            break;
    }
}

YGSize SceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    return {};
}

int32_t SceneNode::GetChildIndex(SceneNode* node) const noexcept {
    int32_t index{0};

    for (const auto& child : YGNodeGetChildren(this->ygNode)) {
        if (YGNodeGetContextAs<SceneNode>(child) == node) {
            return index;
        }
        index++;
    }

    return -1;
}

void SceneNode::RequestPaint() {
    this->scene->RequestPaint(this);
}

void SceneNode::RequestStyleLayout() {
    this->scene->RequestStyleLayout(this);
}

void SceneNode::RequestComposite() {
    this->scene->RequestComposite();
}

const std::vector<SceneNode*>& SceneNode::SortChildrenByStackingOrder() {
    const auto compare = [](SceneNode* a, SceneNode* b) {
        const auto hasTransform {
            [](SceneNode* node) ->bool { return node->style && !node->style->transform.empty();
            }
        };
        const auto zIndex {
            [](SceneNode* node) -> int { return Style::OrEmpty(node->style)->zIndex.AsInt32(0); }
        };
        const auto aHasTransform{ hasTransform(a) };

        if (aHasTransform != hasTransform(b)) {
            // nodes with transform always drawn above nodes with no transform
            //
            // if a has transform and b has no transform, then a is less than b
            // if b has transform and a has no transform, then a is less than b
            return !aHasTransform;
        }

        // by default, nodes have a z-index of 0
        return zIndex(a) < zIndex(b);
    };

    if (!this->sortedChildren.empty()) {
        // already sorted
        return this->sortedChildren;
    }

    this->sortedChildren.reserve(YGNodeGetChildCount(this->ygNode));

    for (const auto& child : YGNodeGetChildren(this->ygNode)) {
        this->sortedChildren.push_back(YGNodeGetContextAs<SceneNode>(child));
    }

    // use stable_sort to preserve the original order of nodes when z-indexes are the same
    std::stable_sort(this->sortedChildren.begin(), this->sortedChildren.end(), compare);

    return this->sortedChildren;
}

bool SceneNode::IsLeaf() const noexcept {
    return this->flags.test(FlagLeaf);
}

bool SceneNode::IsHidden() const noexcept {
    return this->flags.test(FlagHidden);
}

bool SceneNode::IsLayoutOnly() const noexcept {
    return this->flags.test(FlagLayoutOnly);
}

bool SceneNode::HasChildren() const noexcept {
    return !YGNodeGetChildren(this->ygNode).empty();
}

void SceneNode::SetFlag(Flag flag, bool value) noexcept {
    this->flags.set(flag, value);
}

SceneNode* SceneNode::QueryInterface(Napi::Value value) {
    if (!value.IsObject()) {
        return nullptr;
    }

    auto env{ value.Env() };
    HandleScope scope(env);
    auto object{ value.As<Object>() };

    // This is probably expensive, but instanceOf appears to be the cleanest way of getting the SceneNode instance.
    if (object.InstanceOf(BoxSceneNode::GetClass(env))) {
        return BoxSceneNode::Cast(value);
    } else if (object.InstanceOf(ImageSceneNode::GetClass(env))) {
        return ImageSceneNode::Cast(value);
    } else if (object.InstanceOf(TextSceneNode::GetClass(env))) {
        return TextSceneNode::Cast(value);
    } else if (object.InstanceOf(LinkSceneNode::GetClass(env))) {
        return LinkSceneNode::Cast(value);
    } else if (object.InstanceOf(RootSceneNode::GetClass(env))) {
        return RootSceneNode::Cast(value);
    }

    return nullptr;
}

YGSize SceneNode::YogaMeasureCallback(
        YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    auto sceneNode{ static_cast<SceneNode*>(nodeRef->getContext()) };

    assert(sceneNode != nullptr);

    return sceneNode->OnMeasure(width, widthMode, height, heightMode);
}

void SceneNode::YogaNodeLayoutEvent(
        const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data) {
    assert(event == Event::NodeLayout);
    assert(node.getContext() != nullptr);

    if (node.getHasNewLayout()) {
        static_cast<ls::SceneNode*>(node.getContext())->OnBoundingBoxChanged();
    }
}

} // namespace ls
