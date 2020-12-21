/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/SceneNode.h>

namespace lse {

class Style;

class RootSceneNode final : public Napi::SafeObjectWrap<RootSceneNode>, public SceneNode {
 public:
  explicit RootSceneNode(const Napi::CallbackInfo& info) : SafeObjectWrap<RootSceneNode>(info) {}
  ~RootSceneNode() override = default;

  static Napi::Function GetClass(Napi::Env env);
  void Constructor(const Napi::CallbackInfo& info) override;

  void OnStylePropertyChanged(StyleProperty property) override;
  void Paint(RenderingContext2D* context) override {}
  void Composite(CompositeContext* composite) override;
};

} // namespace lse
