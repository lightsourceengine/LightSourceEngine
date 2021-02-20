/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/SceneNode.h>

namespace lse {

class RootSceneNode final : public SceneNode {
 public:
  explicit RootSceneNode(napi_env env, Scene* scene);
  ~RootSceneNode() override = default;

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnComposite(CompositeContext* composite) override;
  void OnDetach() override {}
};

} // namespace lse
