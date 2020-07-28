/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include "ImageStore.h"
#include "CompositeContext.h"
#include "PaintContext.h"

namespace ls {

class SceneNode;
class Stage;
class SceneAdapter;
class Renderer;

class Scene : public Napi::SafeObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info);
    virtual ~Scene() = default;

    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    int32_t GetViewportMin() const noexcept { return std::min(this->width, this->height); }
    int32_t GetViewportMax() const noexcept { return std::max(this->width, this->height); }
    float GetRootFontSize() const noexcept { return this->rootFontSize; }

    ImageStore* GetImageStore() const noexcept { return &this->imageStore; }
    Renderer* GetRenderer() const noexcept;

    Stage* GetStage() const { return this->stage; }
    void SetActiveNode(Napi::Value node);
    void QueuePaint(SceneNode* node);

    void QueueAfterLayout(SceneNode* node);
    void QueueBeforeLayout(SceneNode* node);
    void QueueComposite();
    void QueueRootFontSizeChange(float rootFontSize);
    void Remove(SceneNode* node);

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Frame(const Napi::CallbackInfo& info);
    Napi::Value GetRoot(const Napi::CallbackInfo& info);
    Napi::Value GetStage(const Napi::CallbackInfo& info);
    Napi::Value GetAdapter(const Napi::CallbackInfo& info);

 private:
    mutable ImageStore imageStore;
    SceneNode* root{};
    Stage* stage{};
    SceneAdapter* adapter{};
    float rootFontSize{0};
    int32_t width{};
    int32_t height{};
    bool isSizeDirty{true};
    bool isRootFontSizeDirty{true};
    bool isAttached{false};
    bool hasCompositeRequest{false};
    std::unordered_set<SceneNode*> paintRequests;
    std::unordered_set<SceneNode*> afterLayoutRequests;
    std::unordered_set<SceneNode*> beforeLayoutRequests;
    CompositeContext compositeContext;
    PaintContext paintContext;

    friend Napi::SafeObjectWrap<Scene>;
};

} // namespace ls
