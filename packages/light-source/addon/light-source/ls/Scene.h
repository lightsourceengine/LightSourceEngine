/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/types.h>
#include <ls/CompositeContext.h>
#include <ls/StyleEnums.h>
#include <ls/StyleResolver.h>
#include <ls/RenderingContext2D.h>

#include <algorithm>
#include <memory>
#include <unordered_set>

namespace ls {

class GraphicsContext;
class Renderer;
class RootSceneNode;
class SceneNode;

/**
 * Manages the SceneNode graph and renders frames to the screen.
 */
class Scene {
 public:
    ~Scene();

    void Attach() noexcept;
    void Detach() noexcept;
    void Destroy() noexcept;

    void Frame();

    void SetRoot(RootSceneNode* root);
    Stage* GetStage() const noexcept { return this->stage.get(); }
    void SetStage(const StageRef& stage);
    void SetGraphicsContext(GraphicsContext* graphicsContext);
    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    int32_t GetViewportMin() const noexcept { return this->viewportMin; }
    int32_t GetViewportMax() const noexcept { return this->viewportMax; }
    float GetRootFontSize() const noexcept { return this->rootFontSize; }
    const StyleResolver& GetStyleResolver() const noexcept { return this->styleResolver; }
    Renderer* GetRenderer() const noexcept;

    void OnRootFontSizeChange() noexcept;
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
    void CompositePreorder(SceneNode* node, CompositeContext* context);

 private:
    SceneNode* root{};
    StageRef stage{};
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
