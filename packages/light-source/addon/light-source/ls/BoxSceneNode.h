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
#include <ls/Texture.h>

namespace ls {

class BoxSceneNode final : public Napi::SafeObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    BoxSceneNode(const Napi::CallbackInfo& info) : SafeObjectWrap<BoxSceneNode>(info) {}
    ~BoxSceneNode() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;

    void OnStylePropertyChanged(StyleProperty property) override;
    void OnBoundingBoxChanged() override;
    void OnStyleLayout() override;
    void Paint(RenderingContext2D* context) override;
    void Composite(CompositeContext* composite) override;
    void Destroy() override;

 private:
    void UpdateBackgroundImage(const std::string& backgroundUri);
    void ClearBackgroundImageResource();
    Rect GetBackgroundClipBox(StyleBackgroundClip value) const noexcept;

 private:
    Image* backgroundImage{};
    ImageRect backgroundImageRect{};
    Texture paintTarget{};
};

} // namespace ls
