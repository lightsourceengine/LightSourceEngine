/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cmath>
#include <std20/numbers>

namespace lse {

/**
 * This is a 3x3 2D transformation matrix for translation, rotation and scaling graphics operations.
 */
struct Matrix {
  // 3x3 matrix layout

  float a, b, x; // row 1
  float c, d, y; // row 2
  //    0, 0, 1  // row 3

  // Factory methods

  static Matrix Identity() noexcept {
    return {
        1, 0, 0,
        0, 1, 0,
    };
  }

  static Matrix Rotate(const float rad) noexcept {
    return {
        std::cos(rad), -std::sin(rad), 0,
        std::sin(rad), std::cos(rad), 0,
    };
  }

  static Matrix Translate(const float tx, const float ty) noexcept {
    return {
        1, 0, tx,
        0, 1, ty,
    };
  }

  static Matrix Scale(const float sx, const float sy) noexcept {
    return {
        sx, 0, 0,
        0, sy, 0,
    };
  }

  // Decomposition methods

  /**
   * Decompose axis angle value in radians.
   */
  float GetAxisAngle() const noexcept {
    return std::atan2(c, d);
  }

  /**
   * Decompose axis angle value in degrees ([0,360).
   */
  float GetAxisAngleDeg() const noexcept {
    // + 180 => (-360, 0]
    // negating atan2 params flips the range [0, 360)
    return std::atan2(-c, -d) / std20::pi_v<float> * 180.f + 180.f;
  }

  /**
   * Decompose x-axis scale factor. This will always be a positive value, even if the matrix was created with a
   * negative scale.
   */
  float GetScaleX() const noexcept {
    return std::sqrt(a * a + c * c);
  }

  /**
   * Decompose y-axis scale factor. This will always be a positive value, even if the matrix was created with a
   * negative scale.
   */
  float GetScaleY() const noexcept {
    return std::sqrt(b * b + d * d);
  }

  /**
   * Decompose x-axis translation.
   */
  float GetTranslateX() const noexcept {
    return x;
  }

  /**
   * Decompose y-axis translation.
   */
  float GetTranslateY() const noexcept {
    return y;
  }

  // Operators

  Matrix operator*(const Matrix& other) const noexcept {
    // 3x3 multiplication reduced because row 3 is always 0, 0, 1
    return {
        a * other.a + b * other.c, a * other.b + b * other.d, a * other.x + b * other.y + x,
        c * other.a + d * other.c, c * other.b + d * other.d, c * other.x + d * other.y + y,
    };
  }

  Matrix& operator*=(const Matrix& other) noexcept {
    *this = *this * other;
    return *this;
  }
};

} // namespace lse
