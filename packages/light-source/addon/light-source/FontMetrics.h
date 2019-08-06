/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <stb_truetype.h>
#include <memory>

namespace ls {

struct CodepointMetrics {
    float xOffset;
    float yOffset;
    float width;
    float height;
    float xAdvance;
};

class FontMetrics {
 public:
    FontMetrics(std::shared_ptr<stbtt_fontinfo> fontInfo, int32_t fontSize);
    ~FontMetrics() = default;

    bool GetCodepointMetrics(int32_t codepoint, CodepointMetrics* out) const;
    void GetCodepointMetricsOrFallback(int32_t codepoint, CodepointMetrics* out) const;
    float GetKerning(int32_t codepoint, int32_t nextCodepoint);
    float GetAscent() const { return this->ascent; }
    float GetLineHeight() const { return this->lineHeight; }
    int32_t GetFallbackCodepoint() const { return this->fallbackCodepoint; }

 private:
    float lineHeight{};
    float ascent{};
    float scale{};
    int32_t fallbackCodepoint{0};
    std::shared_ptr<stbtt_fontinfo> fontInfo;
};

} // namespace ls
