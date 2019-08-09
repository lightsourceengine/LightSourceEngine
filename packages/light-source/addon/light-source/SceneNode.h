/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Yoga.h>
#include <vector>
#include <map>
#include <Renderer.h>
#include "Style.h"

namespace ls {

class Scene;
class SceneNode;
class Style;

class SceneNode {
 public:
    explicit SceneNode(const Napi::CallbackInfo& info);
    virtual ~SceneNode() = default;

    static Napi::Value GetInstanceCount(const Napi::CallbackInfo& info);

    Napi::Value GetX(const Napi::CallbackInfo& info);
    Napi::Value GetY(const Napi::CallbackInfo& info);
    Napi::Value GetWidth(const Napi::CallbackInfo& info);
    Napi::Value GetHeight(const Napi::CallbackInfo& info);
    Napi::Value GetParent(const Napi::CallbackInfo& info);
    Napi::Value GetScene(const Napi::CallbackInfo& info);
    Napi::Value GetChildren(const Napi::CallbackInfo& info);

    Napi::Value GetStyle(const Napi::CallbackInfo& info);
    void SetStyle(const Napi::CallbackInfo& info, const Napi::Value& value);

    void AppendChild(const Napi::CallbackInfo& info);
    void InsertBefore(const Napi::CallbackInfo& info);
    void RemoveChild(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

    void Destroy();
    void SyncStyleRecursive();
    void Layout(float width, float height, bool recalculate);
    Style* GetStyleOrEmpty() const { return this->style ? this->style : Style::Empty(); }

    virtual void Paint(Renderer* renderer);
    virtual Napi::Reference<Napi::Object>* AsReference() = 0;

 protected:
     template<typename T>
     static std::vector<Napi::ClassPropertyDescriptor<T>> Extend(Napi::Env env,
         const std::initializer_list<Napi::ClassPropertyDescriptor<T>>& subClassProperties);
    void SetParent(SceneNode* newParent);
    virtual void ApplyStyle(Style* style);
    virtual void DestroyRecursive();
    virtual void AppendChild(SceneNode* child);
    void InsertBefore(SceneNode* child, SceneNode* before);
    void RemoveChild(SceneNode* child);
    void RefreshStyleRecursive();
    void ValidateInsertCandidate(SceneNode* child);

 protected:
    static int instanceCount;
    std::vector<SceneNode*> children{};
    YGNodeRef ygNode{};
    Scene* scene{};
    SceneNode* parent{};
    Style* style{};
};

template<typename T>
std::vector<Napi::ClassPropertyDescriptor<T>> SceneNode::Extend(Napi::Env env,
        const std::initializer_list<Napi::ClassPropertyDescriptor<T>>& subClassProperties) {
    std::vector<Napi::ClassPropertyDescriptor<T>> result = {
        Napi::ObjectWrap<T>::InstanceValue("focusable", Napi::Boolean::New(env, false), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("waypoint", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceAccessor("x", &SceneNode::GetX, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("y", &SceneNode::GetY, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("width", &SceneNode::GetWidth, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("height", &SceneNode::GetHeight, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("parent", &SceneNode::GetParent, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("children", &SceneNode::GetChildren, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("scene", &SceneNode::GetScene, nullptr),
        Napi::ObjectWrap<T>::InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),
        Napi::ObjectWrap<T>::InstanceMethod("destroy", &SceneNode::Destroy),
        Napi::ObjectWrap<T>::InstanceMethod("appendChild", &SceneNode::AppendChild),
        Napi::ObjectWrap<T>::InstanceMethod("insertBefore", &SceneNode::InsertBefore),
        Napi::ObjectWrap<T>::InstanceMethod("removeChild", &SceneNode::RemoveChild),
    };

    for (auto& property : subClassProperties) {
        result.push_back(property);
    }

    return result;
}

} // namespace ls
