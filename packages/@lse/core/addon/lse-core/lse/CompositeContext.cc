/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/CompositeContext.h>

#include <std17/algorithm>
#include <lse/Renderer.h>

namespace lse {

CompositeContext::CompositeContext() {
    this->matrix.reserve(16);
    this->clipRect.reserve(16);
    this->opacity.reserve(16);

    this->matrix.emplace_back(Matrix::Identity());
    this->opacity.push_back(1.0f);
}

void CompositeContext::Reset() {
    this->matrix.resize(1);
    this->opacity.resize(1);
    this->clipRect.clear();
}

void CompositeContext::PushMatrix(const Matrix& m) {
    this->matrix.push_back(this->matrix.back() * m);
}

void CompositeContext::PopMatrix() {
    this->matrix.pop_back();
}

const Matrix& CompositeContext::CurrentMatrix() const noexcept {
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

    this->renderer->EnabledClipping(this->clipRect.back());
}

void CompositeContext::PopClipRect() {
    this->renderer->DisableClipping();
    this->clipRect.pop_back();
}

const Rect& CompositeContext::CurrentClipRect() const noexcept {
    return this->clipRect.back();
}

void CompositeContext::PushOpacity(float value) {
    this->opacity.push_back(this->opacity.back() * std17::clamp(value, 0.f, 1.f));
}

void CompositeContext::PopOpacity() {
    this->opacity.pop_back();
}

uint8_t CompositeContext::CurrentOpacityAlpha() const noexcept {
    return static_cast<uint8_t>(this->opacity.back() * 255.f);
}

float CompositeContext::CurrentOpacity() const noexcept {
    return this->opacity.back();
}

} // namespace lse
