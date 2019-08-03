/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextBlock.h"

namespace ls {

TextBlock::TextBlock() {
}

void TextBlock::SetText(const std::string& text) {
    if (this->text != text) {
        this->text = text;
        this->isDirty = true;
    }
}

void TextBlock::SetText(std::string&& text) {
    if (this->text != text) {
        this->text = std::move(text);
        this->isDirty = true;
    }
}

void TextBlock::SetTextOverflow(StyleTextOverflow textOverflow) {
    if (this->textOverflow != textOverflow) {
        this->textOverflow = textOverflow;
        this->isDirty = true;
    }
}

void TextBlock::SetTextTransform(StyleTextTransform textTransform) {
    if (this->textTransform != textTransform) {
        this->textTransform = textTransform;
        this->isDirty = true;
    }
}

void TextBlock::SetTextAlign(StyleTextAlign textAlign) {
    if (this->textAlign != textAlign) {
        this->textAlign = textAlign;
        this->isDirty = true;
    }
}

void TextBlock::SetMaxLines(int32_t maxLines) {
    if (this->maxLines != maxLines) {
        this->maxLines = maxLines;
        this->isDirty = true;
    }
}

void TextBlock::Layout() {
    this->isDirty = false;
}

void TextBlock::Paint(Renderer* renderer) {
}

} // namespace ls
