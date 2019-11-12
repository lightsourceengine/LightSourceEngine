/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Yoga.h>
#include <vector>
#include <map>
#include <napi-ext.h>
#include "StyleEnums.h"

namespace ls {

class Scene;
class SceneNode;
class Style;
class Renderer;
class CompositeContext;
class Texture;

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
    void Focus(const Napi::CallbackInfo& info);
    void Blur(const Napi::CallbackInfo& info);

    void Destroy();
    void Layout(float width, float height);

    virtual void OnPropertyChanged(StyleProperty property);
    virtual void BeforeLayout() = 0;
    virtual void AfterLayout() = 0;
    virtual void Paint(Renderer* renderer) = 0;
    virtual void Composite(CompositeContext* context);

    virtual Napi::Reference<Napi::Object>* AsReference() noexcept = 0;

    template<typename Callable>
    static void Visit(SceneNode* node, const Callable& func);

 protected:
    template<typename T>
    static std::vector<Napi::ClassPropertyDescriptor<T>> Extend(const Napi::Env& env,
         const std::initializer_list<Napi::ClassPropertyDescriptor<T>>& subClassProperties);
    void SetParent(SceneNode* newParent);
    void InsertBefore(SceneNode* child, SceneNode* before);
    void RemoveChild(SceneNode* child);
    void ValidateInsertCandidate(SceneNode* child);
    virtual void DestroyRecursive();
    virtual void AppendChild(SceneNode* child);
    Style* GetStyleOrEmpty() const noexcept;
    bool InitLayerRenderTarget(Renderer* renderer, int32_t width, int32_t height);
    bool InitLayerSoftwareRenderTarget(Renderer* renderer, int32_t width, int32_t height);
    void QueuePaint();
    void QueueAfterLayout();
    void QueueBeforeLayout();
    void QueueComposite();
    const std::vector<SceneNode*>& SortChildrenByStackingOrder();
    bool HasTransform() const noexcept;
    int32_t GetZIndex() const noexcept;

 protected:
    static int instanceCount;
    std::vector<SceneNode*> children;
    std::vector<SceneNode*> sortedChildren;
    std::shared_ptr<Texture> layer;
    YGNodeRef ygNode{};
    Scene* scene{};
    SceneNode* parent{};
    Style* style{};

    friend Style;
    friend Scene;
};

template<typename Callable>
void SceneNode::Visit(SceneNode* node, const Callable& func) {
    func(node);

    for (auto child : node->children) {
        Visit(child, func);
    }
}

template<typename T>
std::vector<Napi::ClassPropertyDescriptor<T>> SceneNode::Extend(const Napi::Env& env,
        const std::initializer_list<Napi::ClassPropertyDescriptor<T>>& subClassProperties) {
    std::vector<Napi::ClassPropertyDescriptor<T>> result = {
        Napi::ObjectWrap<T>::InstanceValue("focusable", Napi::Boolean::New(env, false), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onKeyUp", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onKeyDown", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onAxisMotion", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onDeviceButtonUp", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onDeviceButtonDown", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onDeviceAxisMotion", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onFocus", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue("onBlur", env.Null(), napi_writable),
        Napi::ObjectWrap<T>::InstanceValue(
            Napi::SymbolFor(env, "hasFocus"), Napi::Boolean::New(env, false), napi_writable),
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
        Napi::ObjectWrap<T>::InstanceMethod("focus", &SceneNode::Focus),
        Napi::ObjectWrap<T>::InstanceMethod("blur", &SceneNode::Blur),
    };

    for (auto& property : subClassProperties) {
        result.push_back(property);
    }

    return result;
}

} // namespace ls
