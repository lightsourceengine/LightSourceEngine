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

#include <vector>
#include <lse/Matrix.h>
#include <lse/Rect.h>
#include <lse/Renderer.h>

namespace lse {

class CompositeContext {
 public:
  CompositeContext();

  void Reset(Renderer* renderer);

  void PushMatrix(const Matrix& m);
  void PopMatrix();
  const Matrix& CurrentMatrix() const noexcept;

  void PushClipRect(const Rect& rect);
  void PopClipRect();
  const Rect& CurrentClipRect() const noexcept;

  void PushOpacity(float opacity);
  void PopOpacity();
  float CurrentOpacity() const noexcept;
  uint8_t CurrentOpacityAlpha() const noexcept;

  RenderTransform CurrentRenderTransform() const noexcept;

 public:
  Renderer* renderer{};

 private:
  std::vector<Matrix> matrix;
  std::vector<Rect> clipRect;
  std::vector<float> opacity;
};

} // namespace lse
