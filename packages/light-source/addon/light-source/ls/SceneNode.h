/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/yoga-ext.h>
#include <event/event.h>
#include <napi-ext.h>
#include <ls/StyleEnums.h>
#include <ls/Resources.h>
#include <bitset>

namespace ls {

class CompositeContext;
class GraphicsContext;
class Renderer;
class RenderingContext2D;
class Scene;
class SceneNode;
class Stage;
class Style;
class Texture;

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

    virtual void OnStylePropertyChanged(StyleProperty property);
    virtual void OnBoundingBoxChanged() {}
    virtual void OnStyleLayout() {}

    virtual YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
    virtual void Paint(RenderingContext2D* context) = 0;
    virtual void Composite(CompositeContext* composite) = 0;
    virtual void Destroy();

    Stage* GetStage() const noexcept;
    SceneNode* GetParent() const noexcept;
    bool IsLeaf() const noexcept;
    bool IsHidden() const noexcept;
    bool IsLayoutOnly() const noexcept;

    bool HasChildren() const noexcept;

    static SceneNode* QueryInterface(Napi::Value value);

    template<typename Callable>
    static void Visit(SceneNode* node, const Callable& func);

    static YGSize YogaMeasureCallback(
        YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

    static void YogaNodeLayoutEvent(
        const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data);

    static Napi::Value GetInstanceCount(const Napi::CallbackInfo& info);

 protected:
    enum Flag : uint32_t {
        FlagHidden,
        FlagLayoutOnly,
        FlagLeaf
    };

    template<typename T>
    static std::vector<napi_property_descriptor> Extend(const Napi::Env& env,
        const std::initializer_list<napi_property_descriptor>& subClassProperties);

    void SceneNodeConstructor(const Napi::CallbackInfo& info);
    void RequestPaint();
    void RequestStyleLayout();
    void RequestComposite();
    const std::vector<SceneNode*>& SortChildrenByStackingOrder();
    void SetFlag(Flag flag, bool value) noexcept;

 private:
    void RemoveChild(SceneNode* child) noexcept;
    int32_t GetChildIndex(SceneNode* node) const noexcept;

 protected:
    static int instanceCount;
    YGNodeRef ygNode{};
    Scene* scene{};
    Style* style{};
    std::vector<SceneNode*> sortedChildren;
    std::bitset<8> flags;

    friend Style;
    friend Scene;
};

template<typename Callable>
void SceneNode::Visit(SceneNode* node, const Callable& func) {
    func(node);

    for (const auto& child : YGNodeGetChildren(node->ygNode)) {
        Visit(YGNodeGetContextAs<SceneNode>(child), func);
    }
}

template<typename T>
std::vector<napi_property_descriptor> SceneNode::Extend(const Napi::Env& env,
        const std::initializer_list<napi_property_descriptor>& subClassProperties) {
    std::vector<napi_property_descriptor> result = {
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
        T::InstanceMethod("removeChild", &SceneNode::RemoveChild)
    };

    for (auto& property : subClassProperties) {
        result.push_back(property);
    }

    return result;
}

} // namespace ls
