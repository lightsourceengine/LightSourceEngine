/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "StyleEnums.h"

namespace ls {

const char* StyleFontWeightToString(const StyleFontWeight value) noexcept {
    switch (value) {
        case StyleFontWeightNormal:
            return "normal";
        case StyleFontWeightBold:
            return "bold";
    }
    return "unknown";
}

const char* StyleFontStyleToString(const StyleFontStyle value) noexcept {
    switch (value) {
        case StyleFontStyleNormal:
            return "normal";
        case StyleFontStyleItalic:
            return "italic";
    }
    return "unknown";
}

const char* StyleTextOverflowToString(const StyleTextOverflow value) noexcept {
  switch (value) {
    case StyleTextOverflowNone:
        return "none";
    case StyleTextOverflowClip:
        return "clip";
    case StyleTextOverflowEllipsis:
        return "ellipsis";
  }
  return "unknown";
}

const char* StyleTextAlignToString(const StyleTextAlign value) noexcept {
  switch (value) {
    case StyleTextAlignLeft:
        return "left";
    case StyleTextAlignCenter:
        return "center";
    case StyleTextAlignRight:
        return "right";
  }
  return "unknown";
}

const char* StyleObjectFitToString(const StyleObjectFit value) noexcept {
  switch (value) {
    case StyleObjectFitFill:
        return "fill";
    case StyleObjectFitContain:
        return "contain";
    case StyleObjectFitCover:
        return "cover";
    case StyleObjectFitNone:
        return "none";
    case StyleObjectFitScaleDown:
        return "scale-down";
  }
  return "unknown";
}

const char* StyleBackgroundClipToString(const StyleBackgroundClip value) noexcept {
  switch (value) {
    case StyleBackgroundClipBorderBox:
        return "border-box";
    case StyleBackgroundClipPaddingBox:
        return "padding-box";
  }
  return "unknown";
}

const char* StyleTextTransformToString(const StyleTextTransform value) noexcept {
  switch (value) {
    case StyleTextTransformNone:
        return "none";
    case StyleTextTransformUppercase:
        return "uppercase";
    case StyleTextTransformLowercase:
        return "lowercase";
  }
  return "unknown";
}

const char* StyleAnchorToString(const StyleAnchor value) noexcept {
  switch (value) {
    case StyleAnchorTop:
        return "top";
    case StyleAnchorRight:
        return "right";
    case StyleAnchorBottom:
        return "bottom";
    case StyleAnchorLeft:
        return "left";
  }
  return "unknown";
}

} // namespace ls
