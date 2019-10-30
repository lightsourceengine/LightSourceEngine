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

class CompositeContext {
 public:
    CompositeContext();

    void Reset();
    void PushMatrix(const Matrix& m);
    void PopMatrix();
    const Matrix& CurrentMatrix();
    void PushClipRect(const Rect& rect);
    void PopClipRect();
    const Rect& CurrentClipRect();

 private:
    std::vector<Matrix> matrix;
    std::vector<Rect> clipRect;
};

} // namespace ls
