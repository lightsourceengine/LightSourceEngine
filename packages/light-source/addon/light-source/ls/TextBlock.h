/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <blend2d.h>
#include <ls/Paintable.h>

namespace ls {

class FontFace;
class Renderer;
class RenderingContext2D;
class Style;
class StyleResolver;

class TextBlock final : public Paintable {
 public:
    ~TextBlock() override = default;

    void Paint(RenderingContext2D* context, Renderer* renderer) override;

    /**
     * Layout the text according to the style policy and dimensions.
     */
    void Shape(const std::string& utf8, FontFace* fontFace, Style* style, const StyleResolver& resolver,
            float maxWidth, float maxHeight);

    // Calculated bounds of the text. Set after call Shape().

    int32_t Width() const noexcept;
    int32_t Height() const noexcept;
    float WidthF() const noexcept;
    float HeightF() const noexcept;

    /**
     * Reset the bounds.
     */
    void Invalidate() noexcept;

    /**
     * If empty, no bounds are set and the text block cannot be pained.
     */
    bool IsEmpty() const noexcept;

 private:
    std::string text;
    StyleTextAlign textAlign;
    BLGlyphBuffer glyphBuffer{};
    int32_t calculatedWidth{0};
    int32_t calculatedHeight{0};
    BLFont font{};
};

} // namespace ls
