/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
