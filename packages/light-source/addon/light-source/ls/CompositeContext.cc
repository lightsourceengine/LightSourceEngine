/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "CompositeContext.h"

namespace ls {

CompositeContext::CompositeContext() {
    this->matrix.reserve(16);
    this->clipRect.reserve(16);
    this->matrix.emplace_back(Matrix::Identity());
}

void CompositeContext::Reset(Renderer* renderer) {
    this->renderer = renderer;
    this->matrix.resize(1);
    this->clipRect.clear();
}

void CompositeContext::PushMatrix(const Matrix& m) {
    this->matrix.push_back(this->matrix.back() * m);
}

void CompositeContext::PopMatrix() {
    this->matrix.pop_back();
}

const Matrix& CompositeContext::CurrentMatrix() {
    return this->matrix.back();
}

void CompositeContext::PushClipRect(const Rect& rect) {
    if (!this->clipRect.empty()) {
        const auto& current{ this->clipRect.back() };
        auto intersect{ Intersect(rect, current) };

        if (IsEmpty(intersect)) {
            this->clipRect.emplace_back(intersect);
        } else {
            this->clipRect.push_back(current);
        }
    } else {
        this->clipRect.push_back(rect);
    }
}

void CompositeContext::PopClipRect() {
    this->clipRect.pop_back();
}

const Rect& CompositeContext::CurrentClipRect() {
    return this->clipRect.back();
}

} // namespace ls
