/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/Style.h>

namespace lse {

/**
 * Validator for Style property numbers.
 *
 * The isValueOk checks the value field of a StyleValue number. If isValueOk returns false, the StyleValue fails
 * validation. If the value passes, each isUnitSupported function is called until at least 1 function returns true
 * for the StyleValue unit field.
 */
struct StyleValueValidator {
  static constexpr auto MAX_UNIT_CHECKS = 6;

  using IsValueOkFunc = bool (*)(float, StyleNumberUnit);
  using IsUnitSupportedFunc = bool (*)(StyleNumberUnit);

  IsValueOkFunc isValueOk{};
  IsUnitSupportedFunc isUnitSupported[MAX_UNIT_CHECKS]{};
  int32_t isUnitSupportedSize{};

  StyleValueValidator() noexcept = default;
  StyleValueValidator(
      IsValueOkFunc isValueOk,
      const std::initializer_list<IsUnitSupportedFunc>& isUnitSupportedChecks) noexcept;

  bool IsValid(const StyleValue& styleValue) const noexcept;

  explicit operator bool() const noexcept { return this->isValueOk; }
};

/**
 * Logic for validating individual property values.
 */
class StyleValidator {
 public:
  static void Init();

  static bool IsValidValue(StyleProperty property, const char* value) noexcept;
  static bool IsValidValue(StyleProperty property, const StyleValue& value) noexcept;
  static bool IsValidValue(StyleProperty property, int32_t value) noexcept;

 private:
  static StyleValueValidator numberValidators[];
};

} // namespace lse
