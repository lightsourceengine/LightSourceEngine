/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Rect.h>
#include <ls/Resources.h>
#include <ls/SceneNode.h>

namespace ls {

class BoxSceneNode : public Napi::SafeObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    BoxSceneNode(const Napi::CallbackInfo& info) : SafeObjectWrap<BoxSceneNode>(info) {}
    ~BoxSceneNode() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;

    bool IsLeaf() const noexcept override { return false; }
    void OnStylePropertyChanged(StyleProperty property) override;
    void OnBoundingBoxChanged() override;
    void OnStyleLayout() override;
    void Paint(GraphicsContext* graphicsContext) override;
    void Composite(CompositeContext* composite) override;

 private:
    void DestroyRecursive() override;
    void UpdateBackgroundImage(const std::string& backgroundUri);
    void ClearBackgroundImageResource();

 private:
    Image* backgroundImage{};
    ImageRect backgroundImageRect{};
};

} // namespace ls
