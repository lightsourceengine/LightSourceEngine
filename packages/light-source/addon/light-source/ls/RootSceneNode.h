/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/SceneNode.h>

namespace ls {

class Style;

class RootSceneNode : public Napi::SafeObjectWrap<RootSceneNode>, public SceneNode {
 public:
    explicit RootSceneNode(const Napi::CallbackInfo& info) : SafeObjectWrap<RootSceneNode>(info) {}
    ~RootSceneNode() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;

    bool IsLeaf() const noexcept override { return false; }
    void OnStylePropertyChanged(StyleProperty property) override;
    void Paint(GraphicsContext* graphicsContext) override {}
    void Composite(CompositeContext* composite) override;
};

} // namespace ls
