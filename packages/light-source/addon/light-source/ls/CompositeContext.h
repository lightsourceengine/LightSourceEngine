/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <ls/Matrix.h>
#include <ls/Rect.h>

namespace ls {

class Renderer;

class CompositeContext {
 public:
    CompositeContext();

    void Reset(Renderer* renderer);

    void PushMatrix(const Matrix& m);
    void PopMatrix();
    const Matrix& CurrentMatrix();

    void PushClipRect(const Rect& rect);
    void PopClipRect();
    const Rect& CurrentClipRect();

    void PushOpacity(float opacity);
    void PopOpacity();
    float CurrentOpacity() const;
    uint8_t CurrentOpacityAlpha() const;

 public:
    Renderer* renderer;

 private:
    std::vector<Matrix> matrix;
    std::vector<Rect> clipRect;
    std::vector<float> opacity;
};

} // namespace ls
