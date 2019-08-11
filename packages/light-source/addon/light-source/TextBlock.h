/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "StyleEnums.h"
#include "Renderer.h"
#include "FontMetrics.h"

namespace ls {

class Renderer;
class FontSampleResource;

class TextBlock {
 private:
    struct CodepointRect : Rect {
        CodepointRect() {}
        CodepointRect(float x, float y, float width, float height, int32_t codepoint)
            : codepoint(codepoint) { this->x = x; this->y = y; this->width = width; this->height = height; }

        int32_t codepoint;
    };

 public:
    TextBlock();
    ~TextBlock();

    void SetFont(FontSampleResource* font);
    void SetText(const std::string& text);
    void SetText(std::string&& text);

    void SetTextOverflow(StyleTextOverflow textOverflow);
    void SetTextAlign(StyleTextAlign textAlign);
    void SetMaxLines(int32_t maxLines);

    bool IsDirty() const { return this->isDirty; }
    float GetComputedWidth() const { return this->computedWidth; }
    float GetComputedHeight() const { return this->computedHeight; }

    void Layout(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
    void Paint(Renderer* renderer, float x, float y, int64_t color);

 private:
    void MarkDirty();

 private:
    FontSampleResource* font{};
    bool isDirty{false};
    float computedWidth{0};
    float computedHeight{0};
    float measuredWidth{};
    int32_t measuredWidthMode{};
    float measuredHeight{};
    int32_t measuredHeightMode{};
    std::string text;
    StyleTextOverflow textOverflow{};
    StyleTextAlign textAlign{};
    int32_t maxLines{};
    mutable std::vector<CodepointRect> quads;

    friend class LayoutLine;
};

} // namespace ls
