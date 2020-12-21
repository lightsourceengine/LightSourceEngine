/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <lse/Matrix.h>
#include <lse/Rect.h>

namespace lse {

class Renderer;

class CompositeContext {
 public:
  CompositeContext();

  void Reset();

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

 public:
  Renderer* renderer{};

 private:
  std::vector<Matrix> matrix;
  std::vector<Rect> clipRect;
  std::vector<float> opacity;
};

} // namespace lse
