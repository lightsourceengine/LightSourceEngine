/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/CompositeContext.h>
#include <ls/StyleEnums.h>
#include <ls/StyleResolver.h>
#include <ls/RenderingContext2D.h>
#include <napi-ext.h>

#include <algorithm>
#include <memory>
#include <unordered_set>

namespace ls {

class Renderer;
class GraphicsContext;
class SceneNode;
class Stage;

class Scene : public Napi::SafeObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<Scene>(info) {}
    ~Scene() override;

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
    const StyleResolver& GetStyleResolver() const noexcept { return this->styleResolver; }

    void OnRootFontSizeChange() noexcept;

    Renderer* GetRenderer() const noexcept;
    Stage* GetStage() const noexcept { return this->stage; }
    void SetActiveNode(Napi::Value node);

    void RequestPaint(SceneNode* node);
    void RequestStyleLayout(SceneNode* node);
    void RequestComposite();
    void Remove(SceneNode* node);

 private:
    void PropagateViewportAndRootFontSizeChanges();
    void ComputeBoundingBoxLayout();
    void ExecuteStyleLayoutRequests();
    void ExecutePaintRequests();
    void Composite();
    void RemoveInternalReferences() noexcept;

 private:
    SceneNode* root{};
    Stage* stage{};
    GraphicsContext* graphicsContext{};
    int32_t width{};
    int32_t height{};
    int32_t viewportMin{};
    int32_t viewportMax{};
    StyleResolver styleResolver{};
    float rootFontSize{DEFAULT_REM_FONT_SIZE};
    bool isViewportSizeDirty{true};
    bool isRootFontSizeDirty{false};
    bool isAttached{false};
    bool hasCompositeRequest{false};
    std::unordered_set<SceneNode*> paintRequests;
    std::unordered_set<SceneNode*> styleLayoutRequests;
    CompositeContext compositeContext;
    RenderingContext2D renderingContext2D{};
};

} // namespace ls
