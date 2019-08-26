/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Font.h"
#include "Surface.h"
#include <YGEnums.h>
#include <memory>

namespace ls {

class TextLine {
 public:
    TextLine(std::shared_ptr<Font> font); // NOLINT(runtime/explicit)
    TextLine(const TextLine& other) = default;
    TextLine(TextLine&& other);

     bool Append(int32_t codepoint, float maxWidth);
     void Paint(float x, float y, const Surface& target);
     float Width() const { return this->width; }

    TextLine Break(bool hardLineBreak);
    void Finalize();
    void Ellipsize(float maxWidth);
    bool IsEmpty() const { return this->chars.empty(); }
 private:
    float CalculateWidth() const;

 private:
     std::shared_ptr<Font> font;
     float width{0};
     std::vector<int32_t> chars;
};

} // namespace ls
