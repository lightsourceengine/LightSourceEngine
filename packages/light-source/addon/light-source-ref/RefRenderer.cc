/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefRenderer.h"

namespace ls {

uint32_t RefRenderer::nextTextureId{ 1 };

RefRenderer::RefRenderer() {
}

RefRenderer::~RefRenderer() {
}

int32_t RefRenderer::GetWidth() const {
    return 0;
}

int32_t RefRenderer::GetHeight() const {
    return 0;
}

void RefRenderer::Reset() {
}

void RefRenderer::Present() {
}

void RefRenderer::Shift(float x, float y) {
}

void RefRenderer::Unshift() {
}

void RefRenderer::PushClipRect(const Rect& rect) {
}

void RefRenderer::PopClipRect() {
}

void RefRenderer::DrawFillRect(const Rect& rect, const int64_t fillColor) {
}

void RefRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) {
}

void RefRenderer::DrawImage(const uint32_t textureId, const Rect& rect, const int64_t tintColor) {
}

void RefRenderer::DrawImage(const uint32_t textureId, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) {
}

void RefRenderer::DrawQuad(const uint32_t textureId, const Rect& srcRect, const Rect& destRect,
        const int64_t tintColor) {
}

uint32_t RefRenderer::CreateTexture(const uint8_t* source, PixelFormat sourceFormat,
        const int32_t width, const int32_t height) {
    return nextTextureId++;
}

void RefRenderer::DestroyTexture(const uint32_t textureId) {
}

} // namespace ls
