/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "StyleEnums.h"

namespace ls {

class Renderer;

class TextBlock {
 public:
    TextBlock();
    virtual ~TextBlock() = default;

    std::string GetText() const { return this->text; }
    void SetText(const std::string& text);
    void SetText(std::string&& text);

    void SetTextOverflow(StyleTextOverflow textOverflow);
    void SetTextTransform(StyleTextTransform textTransform);
    void SetTextAlign(StyleTextAlign textAlign);
    void SetMaxLines(int32_t maxLines);

    bool IsDirty() const { return this->isDirty; }

    void Layout();
    void Paint(Renderer* renderer);

    float GetComputedWidth() const { return this->computedWidth; }
    float GetComputedHeight() const { return this->computedHeight; }

 private:
    bool isDirty{false};
    float computedWidth{0};
    float computedHeight{0};
    std::string text;
    StyleTextOverflow textOverflow{};
    StyleTextTransform textTransform{};
    StyleTextAlign textAlign{};
    int32_t maxLines{};
};

} // namespace ls
