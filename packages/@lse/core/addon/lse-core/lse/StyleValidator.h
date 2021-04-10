/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
