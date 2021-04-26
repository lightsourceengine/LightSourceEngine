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

#include <lse/CompositeContext.h>

#include <std17/algorithm>
#include <lse/Renderer.h>

namespace lse {

CompositeContext::CompositeContext() {
  this->matrix.reserve(16);
  this->clipRect.reserve(16);
  this->opacity.reserve(16);

  this->Reset(nullptr);
}

void CompositeContext::Reset(Renderer* renderer) {
  this->renderer = renderer;

  this->matrix.clear();
  this->opacity.clear();
  this->clipRect.clear();

  this->matrix.emplace_back(Matrix::Identity());
  this->opacity.push_back(1.0f);
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

RenderTransform CompositeContext::CurrentRenderTransform() const noexcept {
  const auto& m{this->matrix.back()};

// TODO: turn off scale and rotation until composite can render scale/rotate
// return { m.x, m.y, m.GetScaleX(), m.GetScaleY(), m.GetAxisAngleDeg() };
  return { m.x, m.y, 1.f, 1.f, 0.f };
}

} // namespace lse
