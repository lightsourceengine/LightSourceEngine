/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Yoga.h>
#include <event/event.h>
#include <napi-ext.h>
#include <ls/StyleEnums.h>
#include <ls/Resources.h>

namespace ls {

class Scene;
class SceneNode;
class Stage;
class Style;
class Renderer;
class CompositeContext;
class GraphicsContext;
class Texture;

enum SceneNodeType {
    SceneNodeTypeRoot,
    SceneNodeTypeBox,
    SceneNodeTypeImage,
    SceneNodeTypeText,
    SceneNodeTypeLink
};

class SceneNode : public virtual Napi::SafeObjectWrapReference {
 public:
    ~SceneNode() override = default;

    Napi::Value GetX(const Napi::CallbackInfo& info);
    Napi::Value GetY(const Napi::CallbackInfo& info);
    Napi::Value GetWidth(const Napi::CallbackInfo& info);
    Napi::Value GetHeight(const Napi::CallbackInfo& info);
    Napi::Value GetParent(const Napi::CallbackInfo& info);
    Napi::Value GetScene(const Napi::CallbackInfo& info);
    Napi::Value GetChildren(const Napi::CallbackInfo& info);
    Napi::Value GetStyle(const Napi::CallbackInfo& info);
    void SetStyle(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetHidden(const Napi::CallbackInfo& info);
    void SetHidden(const Napi::CallbackInfo& info, const Napi::Value& value);

    void AppendChild(const Napi::CallbackInfo& info);
    void InsertBefore(const Napi::CallbackInfo& info);
    void RemoveChild(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Focus(const Napi::CallbackInfo& info);
    void Blur(const Napi::CallbackInfo& info);

    Stage* GetStage() const noexcept;
    void Destroy();

    virtual void OnStylePropertyChanged(StyleProperty property);
    virtual void OnBoundingBoxChanged() {}
    virtual void OnStyleLayout() {}

    virtual YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
    virtual void Paint(GraphicsContext* graphicsContext) = 0;
    virtual void Composite(CompositeContext* composite);
    virtual bool IsLeaf() const noexcept { return true; }

    static SceneNode* QueryInterface(Napi::Value value);
    static void SetType(Napi::Value value, SceneNodeType type);

    template<typename Callable>
    static void Visit(SceneNode* node, const Callable& func);

    static YGSize YogaMeasureCallback(
        YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

    static void YogaNodeLayoutEvent(
        const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data);

    static Napi::Value GetInstanceCount(const Napi::CallbackInfo& info);

 protected:
    template<typename T>
    static std::vector<napi_property_descriptor> Extend(const Napi::Env& env,
        const std::initializer_list<napi_property_descriptor>& subClassProperties);

    void SceneNodeConstructor(const Napi::CallbackInfo& info, SceneNodeType type);
    void SetParent(SceneNode* newParent);
    void InsertBefore(const Napi::Env& env, SceneNode* child, SceneNode* before);
    void RemoveChild(SceneNode* child);
    void CanAddChild(const Napi::Env& env, SceneNode* child);
    virtual void DestroyRecursive();
    void AppendChild(SceneNode* child);
    Style* GetStyleOrEmpty() const noexcept;
    bool InitLayerRenderTarget(Renderer* renderer, int32_t width, int32_t height);
    bool InitLayerSoftwareRenderTarget(Renderer* renderer, int32_t width, int32_t height);
    void RequestPaint();
    void RequestStyleLayout();
    void RequestComposite();
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
    bool isHidden{false};

    friend Style;
    friend Scene;
    friend class BoxSceneNode;
};

template<typename Callable>
void SceneNode::Visit(SceneNode* node, const Callable& func) {
    func(node);

    for (auto child : node->children) {
        Visit(child, func);
    }
}

template<typename T>
std::vector<napi_property_descriptor> SceneNode::Extend(const Napi::Env& env,
        const std::initializer_list<napi_property_descriptor>& subClassProperties) {
    std::vector<napi_property_descriptor> result = {
        T::InstanceValue("focusable", Napi::Boolean::New(env, false), napi_writable),
        T::InstanceValue("onKeyUp", env.Null(), napi_writable),
        T::InstanceValue("onKeyDown", env.Null(), napi_writable),
        T::InstanceValue("onAxisMotion", env.Null(), napi_writable),
        T::InstanceValue("onDeviceButtonUp", env.Null(), napi_writable),
        T::InstanceValue("onDeviceButtonDown", env.Null(), napi_writable),
        T::InstanceValue("onDeviceAxisMotion", env.Null(), napi_writable),
        T::InstanceValue("onFocus", env.Null(), napi_writable),
        T::InstanceValue("onBlur", env.Null(), napi_writable),
        T::InstanceValue(Napi::SymbolFor(env, "hasFocus"), Napi::Boolean::New(env, false), napi_writable),
        T::InstanceAccessor("x", &SceneNode::GetX),
        T::InstanceAccessor("y", &SceneNode::GetY),
        T::InstanceAccessor("width", &SceneNode::GetWidth),
        T::InstanceAccessor("height", &SceneNode::GetHeight),
        T::InstanceAccessor("parent", &SceneNode::GetParent),
        T::InstanceAccessor("children", &SceneNode::GetChildren),
        T::InstanceAccessor("scene", &SceneNode::GetScene),
        T::InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),
        T::InstanceAccessor("hidden", &SceneNode::GetHidden, &SceneNode::SetHidden),
        T::InstanceMethod("destroy", &SceneNode::Destroy),
        T::InstanceMethod("appendChild", &SceneNode::AppendChild),
        T::InstanceMethod("insertBefore", &SceneNode::InsertBefore),
        T::InstanceMethod("removeChild", &SceneNode::RemoveChild),
        T::InstanceMethod("focus", &SceneNode::Focus),
        T::InstanceMethod("blur", &SceneNode::Blur),
    };

    for (auto& property : subClassProperties) {
        result.push_back(property);
    }

    return result;
}

} // namespace ls
