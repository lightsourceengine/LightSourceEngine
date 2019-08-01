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

    Napi::Value GetStyle(const Napi::CallbackInfo& info);

    void SetStyle(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SyncStyleRecursive();

    void AppendChild(const Napi::CallbackInfo& info);
    void InsertBefore(const Napi::CallbackInfo& info);
    void RemoveChild(const Napi::CallbackInfo& info);

    void Destroy(const Napi::CallbackInfo& info);

    void Destroy();

    void Layout(float width, float height, bool recalculate);
    Style* GetStyleOrEmpty() const { return this->style ? this->style : Style::Empty(); }

    virtual void Paint(Renderer* renderer);
    virtual Napi::Reference<Napi::Object>* AsReference() = 0;

 protected:
    void SetParent(SceneNode* newParent);
    virtual void ApplyStyle(Style* style);
    virtual void DestroyRecursive();
    void RemoveChild(SceneNode* child);
    void RefreshStyleRecursive();

 protected:
    static int instanceCount;
    YGNodeRef ygNode{};
    bool isLeaf{false};
    Scene* scene{};
    SceneNode* parent{};
    Style* style{};
    std::vector<SceneNode*> children{};
};

} // namespace ls
