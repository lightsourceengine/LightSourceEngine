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
#include <ls/CompositeContext.h>
#include <ls/StyleEnums.h>

namespace ls {

class SceneNode;
class Stage;
class Renderer;
class GraphicsContext;

class Scene : public Napi::SafeObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<Scene>(info) {}
    ~Scene() override = default;

    // javascript bindings

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Frame(const Napi::CallbackInfo& info);
    Napi::Value GetRoot(const Napi::CallbackInfo& info);
    Napi::Value GetStage(const Napi::CallbackInfo& info);
    Napi::Value GetGraphicsContext(const Napi::CallbackInfo& info);

    // native interface

    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    int32_t GetViewportMin() const noexcept { return this->viewportMin; }
    int32_t GetViewportMax() const noexcept { return this->viewportMax; }
    float GetRootFontSize() const noexcept { return this->rootFontSize; }

    void OnRootFontSizeChange(float newRootFontSize) noexcept;

    Renderer* GetRenderer() const noexcept;
    Stage* GetStage() const noexcept { return this->stage; }
    void SetActiveNode(Napi::Value node);

    void QueuePaint(SceneNode* node);
    void QueueAfterLayout(SceneNode* node);
    void QueueBeforeLayout(SceneNode* node);
    void QueueComposite();
    void Remove(SceneNode* node);

 private:
    SceneNode* root{};
    Stage* stage{};
    GraphicsContext* graphicsContext{};
    int32_t width{};
    int32_t height{};
    int32_t viewportMin{};
    int32_t viewportMax{};
    bool isViewportSizeDirty{true};
    bool isRootFontSizeDirty{false};
    bool isAttached{false};
    bool hasCompositeRequest{false};
    float rootFontSize{DEFAULT_REM_FONT_SIZE};
    std::unordered_set<SceneNode*> paintRequests;
    std::unordered_set<SceneNode*> afterLayoutRequests;
    std::unordered_set<SceneNode*> beforeLayoutRequests;
    CompositeContext compositeContext;

    friend Napi::SafeObjectWrap<Scene>;
};

} // namespace ls
