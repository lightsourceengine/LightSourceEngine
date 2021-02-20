/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/GraphicsContext.h>

namespace lse {

class RefRenderer;

/**
 * A stub implementation of GraphicsContext that allows Light Source Engine to run
 * in environments without a rendering, such as running headless tests.
 */
class RefGraphicsContext final : public GraphicsContext {
 public:
  RefGraphicsContext(const GraphicsContextConfig& config);

  void Attach() override;
  void Detach() override;

 private:
  std::shared_ptr<RefRenderer> refRenderer;
};

} // namespace lse
