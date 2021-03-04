/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/CStyleUtil.h>

#include <napix.h>
#include <lse/StyleValidator.h>
#include <lse/string-ext.h>
#include <lse/Habitat.h>

namespace lse {
namespace bindings {

constexpr auto kStyleValueUndefinedObjectIndex = 0;
constexpr auto kStyleValueValueIndex = 0;
constexpr auto kStyleValueUnitIndex = 1;

constexpr auto kStyleTransformSpecUndefinedObjectIndex = 0;
constexpr auto kStyleTransformSpecTransformIndex = 0;
constexpr auto kStyleTransformSpecXIndex = 1;
constexpr auto kStyleTransformSpecYIndex = 2;
constexpr auto kStyleTransformSpecAngleIndex = 3;

static std17::optional<color_t> ParseHexHashColorString(const char* str) noexcept;
static std17::optional<StyleValue> ParseStyleNumberString(const char* value) noexcept;

static CStringHashMap<StyleNumberUnit> sUnitMap{
  { "px", StyleNumberUnitPoint },
  { "%", StyleNumberUnitPercent },
  { "vw", StyleNumberUnitViewportWidth },
  { "vh", StyleNumberUnitViewportHeight },
  { "vmin", StyleNumberUnitViewportMin },
  { "vmax", StyleNumberUnitViewportMax },
  { "rem", StyleNumberUnitRootEm },
  { "deg", StyleNumberUnitDegree },
  { "rad", StyleNumberUnitRadian },
  { "grad", StyleNumberUnitGradian },
  { "turn", StyleNumberUnitTurn },
};

static CStringHashMap<uint32_t> sColorMap{
  // from: https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#Color_keywords
  { "aliceblue", 0xFFF0F8FF },
  { "antiquewhite", 0xFFFAEBD7 },
  { "aqua", 0xFF00FFFF },
  { "aquamarine", 0xFF7FFFD4 },
  { "azure", 0xFFF0FFFF },
  { "beige", 0xFFF5F5DC },
  { "bisque", 0xFFFFE4C4 },
  { "black", 0xFF000000 },
  { "blanchedalmond", 0xFFFFEBCD },
  { "blue", 0xFF0000FF },
  { "blueviolet", 0xFF8A2BE2 },
  { "brown", 0xFFA52A2A },
  { "burlywood", 0xFFDEB887 },
  { "cadetblue", 0xFF5F9EA0 },
  { "chartreuse", 0xFF7FFF00 },
  { "chocolate", 0xFFD2691E },
  { "coral", 0xFFFF7F50 },
  { "cornflowerblue", 0xFF6495ED },
  { "cornsilk", 0xFFFFF8DC },
  { "crimson", 0xFFDC143C },
  { "cyan", 0xFF00FFFF },
  { "darkblue", 0xFF00008B },
  { "darkcyan", 0xFF008B8B },
  { "darkgoldenrod", 0xFFB8860B },
  { "darkgray", 0xFFA9A9A9 },
  { "darkgreen", 0xFF006400 },
  { "darkgrey", 0xFFA9A9A9 },
  { "darkkhaki", 0xFFBDB76B },
  { "darkmagenta", 0xFF8B008B },
  { "darkolivegreen", 0xFF556B2F },
  { "darkorange", 0xFFFF8C00 },
  { "darkorchid", 0xFF9932CC },
  { "darkred", 0xFF8B0000 },
  { "darksalmon", 0xFFE9967A },
  { "darkseagreen", 0xFF8FBC8F },
  { "darkslateblue", 0xFF483D8B },
  { "darkslategray", 0xFF2F4F4F },
  { "darkslategrey", 0xFF2F4F4F },
  { "darkturquoise", 0xFF00CED1 },
  { "darkviolet", 0xFF9400D3 },
  { "deeppink", 0xFFFF1493 },
  { "deepskyblue", 0xFF00BFFF },
  { "dimgray", 0xFF696969 },
  { "dimgrey", 0xFF696969 },
  { "dodgerblue", 0xFF1E90FF },
  { "firebrick", 0xFFB22222 },
  { "floralwhite", 0xFFFFFAF0 },
  { "forestgreen", 0xFF228B22 },
  { "fuchsia", 0xFFFF00FF },
  { "gainsboro", 0xFFDCDCDC },
  { "ghostwhite", 0xFFF8F8FF },
  { "gold", 0xFFFFD700 },
  { "goldenrod", 0xFFDAA520 },
  { "gray", 0xFF808080 },
  { "green", 0xFF008000 },
  { "greenyellow", 0xFFADFF2F },
  { "grey", 0xFF808080 },
  { "honeydew", 0xFFF0FFF0 },
  { "hotpink", 0xFFFF69B4 },
  { "indianred", 0xFFCD5C5C },
  { "indigo", 0xFF4B0082 },
  { "ivory", 0xFFFFFFF0 },
  { "khaki", 0xFFF0E68C },
  { "lavender", 0xFFE6E6FA },
  { "lavenderblush", 0xFFFFF0F5 },
  { "lawngreen", 0xFF7CFC00 },
  { "lemonchiffon", 0xFFFFFACD },
  { "lightblue", 0xFFADD8E6 },
  { "lightcoral", 0xFFF08080 },
  { "lightcyan", 0xFFE0FFFF },
  { "lightgoldenrodyellow", 0xFFFAFAD2 },
  { "lightgray", 0xFFD3D3D3 },
  { "lightgreen", 0xFF90EE90 },
  { "lightgrey", 0xFFD3D3D3 },
  { "lightpink", 0xFFFFB6C1 },
  { "lightsalmon", 0xFFFFA07A },
  { "lightseagreen", 0xFF20B2AA },
  { "lightskyblue", 0xFF87CEFA },
  { "lightslategray", 0xFF778899 },
  { "lightslategrey", 0xFF778899 },
  { "lightsteelblue", 0xFFB0C4DE },
  { "lightyellow", 0xFFFFFFE0 },
  { "lime", 0xFF00FF00 },
  { "limegreen", 0xFF32CD32 },
  { "linen", 0xFFFAF0E6 },
  { "magenta", 0xFFFF00FF },
  { "maroon", 0xFF800000 },
  { "mediumaquamarine", 0xFF66CDAA },
  { "mediumblue", 0xFF0000CD },
  { "mediumorchid", 0xFFBA55D3 },
  { "mediumpurple", 0xFF9370DB },
  { "mediumseagreen", 0xFF3CB371 },
  { "mediumslateblue", 0xFF7B68EE },
  { "mediumspringgreen", 0xFF00FA9A },
  { "mediumturquoise", 0xFF48D1CC },
  { "mediumvioletred", 0xFFC71585 },
  { "midnightblue", 0xFF191970 },
  { "mintcream", 0xFFF5FFFA },
  { "mistyrose", 0xFFFFE4E1 },
  { "moccasin", 0xFFFFE4B5 },
  { "navajowhite", 0xFFFFDEAD },
  { "navy", 0xFF000080 },
  { "oldlace", 0xFFFDF5E6 },
  { "olive", 0xFF808000 },
  { "olivedrab", 0xFF6B8E23 },
  { "orange", 0xFFFFA500 },
  { "orangered", 0xFFFF4500 },
  { "orchid", 0xFFDA70D6 },
  { "palegoldenrod", 0xFFEEE8AA },
  { "palegreen", 0xFF98FB98 },
  { "paleturquoise", 0xFFAFEEEE },
  { "palevioletred", 0xFFDB7093 },
  { "papayawhip", 0xFFFFEFD5 },
  { "peachpuff", 0xFFFFDAB9 },
  { "peru", 0xFFCD853F },
  { "pink", 0xFFFFC0CB },
  { "plum", 0xFFDDA0DD },
  { "powderblue", 0xFFB0E0E6 },
  { "purple", 0xFF800080 },
  { "red", 0xFFFF0000 },
  { "rosybrown", 0xFFBC8F8F },
  { "royalblue", 0xFF4169E1 },
  { "saddlebrown", 0xFF8B4513 },
  { "salmon", 0xFFFA8072 },
  { "sandybrown", 0xFFF4A460 },
  { "seagreen", 0xFF2E8B57 },
  { "seashell", 0xFFFFF5EE },
  { "sienna", 0xFFA0522D },
  { "silver", 0xFFC0C0C0 },
  { "skyblue", 0xFF87CEEB },
  { "slateblue", 0xFF6A5ACD },
  { "slategray", 0xFF708090 },
  { "slategrey", 0xFF708090 },
  { "snow", 0xFFFFFAFA },
  { "springgreen", 0xFF00FF7F },
  { "steelblue", 0xFF4682B4 },
  { "tan", 0xFFD2B48C },
  { "teal", 0xFF008080 },
  { "thistle", 0xFFD8BFD8 },
  { "tomato", 0xFFFF6347 },
  { "turquoise", 0xFF40E0D0 },
  { "violet", 0xFFEE82EE },
  { "wheat", 0xFFF5DEB3 },
  { "white", 0xFFFFFFFF },
  { "whitesmoke", 0xFFF5F5F5 },
  { "yellow", 0xFFFFFF00 },
  { "yellowgreen", 0xFF9ACD32 },
  { "rebeccapurple", 0xFF663399 },
  { "transparent", 0 },
};

napi_value BoxColor(napi_env env, const std17::optional<color_t>& color) noexcept {
  if (color.has_value()) {
    return napix::to_value(env, color->value);
  } else {
    return {};
  }
}

std17::optional<color_t> UnboxColor(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};
  napi_typeof(env, value, &type);

  switch (type) {
    case napi_string: {
      // 24 bytes is more than enough space for the longest css color name and # colors
      static constexpr size_t kBufferSize{24};
      char buffer[kBufferSize];

      if (napi_get_value_string_utf8(env, value, buffer, kBufferSize, nullptr) != napi_ok) {
        return {};
      }

      // no utf8 encoded strings in valid css color or hex strings, so this simple tolower is sufficient
      ToLowercase(buffer);

      if (buffer[0] == '#') {
        return ParseHexHashColorString(buffer);
      } else {
        auto it{ sColorMap.find(buffer) };

        if (it != sColorMap.end()) {
          return { it->second };
        }

        return {};
      }
    }
    case napi_number:
      return { napix::as_uint32(env, value, 0) };
    default:
      return {};
  }
}

napi_value BoxStyleValue(napi_env env, const StyleValue& value) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::StyleValue)};

  if (!constructor) {
    return {};
  }

  if (value.IsUndefined()) {
    napi_value undefined{};
    napi_get_element(env, constructor, kStyleValueUndefinedObjectIndex, &undefined);

    return undefined;
  }

  napi_value instance{};
  static constexpr size_t argc{2};
  napi_value argv[argc]{
    napix::to_value_or_null(env, value.value),
    napix::to_value_or_null(env, value.unit)
  };

  napi_new_instance(env, constructor, argc, argv, &instance);

  return instance;
}

std17::optional<StyleValue> UnboxStyleValue(napi_env env, napi_value value) noexcept {
  bool isArray{};

  // use case: style.prop = otherStyle.prop
  // use case: style.prop = [ 100, StyleUnit.POINT ]
  if (Habitat::InstanceOf(env, value, Habitat::Class::StyleValue)
      || (napi_is_array(env, value, &isArray) == napi_ok && isArray)) {
    return StyleValue(
      napix::object_at_or(env, value, kStyleValueValueIndex, kUndefined),
      napix::object_at_or(env, value, kStyleValueUnitIndex, StyleNumberUnitUndefined));
  }

  napi_valuetype type{};
  napi_typeof(env, value, &type);

  switch (type) {
    // use case: style.prop = { value: 100, unit: StyleUnit.POINT }
    case napi_object:
      return StyleValue(napix::object_get_or(env, value, "value", kUndefined),
          napix::object_get_or(env, value, "unit", StyleNumberUnitUndefined));
    // use case: style.prop = '100vw'
    case napi_string: {
      static constexpr size_t kBufferSize{32};
      char buffer[kBufferSize];

      if (napi_get_value_string_utf8(env, value, buffer, kBufferSize, nullptr) != napi_ok) {
        return {};
      }

      return ParseStyleNumberString(ToLowercase(buffer));
    }
    // use case: style.prop = 50
    case napi_number: {
      return StyleValue(napix::as_float(env, value, kUndefined), StyleNumberUnitPoint);
    }
    default: {
      // garbage in, undefined out
      return {};
    }
  }
}

napi_value BoxStyleTransformSpec(napi_env env, const std::vector<StyleTransformSpec>& transform) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::StyleTransformSpec)};

  if (!constructor) {
    return {};
  }

  napi_value result{};

  if (!transform.empty()) {
    result = napix::array_new(env, transform.size());
  }

  if (!result) {
    napi_value undefined{};
    napi_get_element(env, constructor, kStyleTransformSpecUndefinedObjectIndex, &undefined);

    return undefined;
  }

  static constexpr size_t kMaxArgs{3};
  napi_value argv[kMaxArgs]{};
  size_t i{0};

  for (const auto& t : transform) {
    size_t argc{1};

    argv[0] = napix::to_value_or_null(env, t.transform);

    switch (t.transform) {
      case StyleTransformIdentity:
        break;
      case StyleTransformRotate:
        argv[argc++] = BoxStyleValue(env, t.angle);
        break;
      case StyleTransformScale:
      case StyleTransformTranslate:
        argv[argc++] = BoxStyleValue(env, t.x);
        argv[argc++] = BoxStyleValue(env, t.y);
        break;
      default:
        break;
    }

    napi_value instance{};
    napi_new_instance(env, constructor, argc, argv, &instance);

    if (napix::has_pending_exception(env)) {
      return {};
    }

    napi_set_element(env, result, i++, instance);
  }

  return result;
}

std17::optional<StyleTransformSpec> UnboxStyleTransformSpec(
    napi_env env, napi_value value, napi_value constructor) noexcept {
  bool instanceOf{};
  napi_instanceof(env, value, constructor, &instanceOf);

  if (!instanceOf) {
    return {};
  }

  auto transform{napix::object_at_or(env, value, kStyleTransformSpecTransformIndex, -1)};

  if (!IsEnum<StyleTransform>(transform)) {
    return {};
  }

  auto x{UnboxStyleValue(env, napix::object_at(env, value, kStyleTransformSpecXIndex))};
  auto y{UnboxStyleValue(env, napix::object_at(env, value, kStyleTransformSpecYIndex))};
  auto angle{UnboxStyleValue(env, napix::object_at(env, value, kStyleTransformSpecAngleIndex))};
  auto undefined{StyleValue::OfUndefined()};

  return StyleTransformSpec{
      static_cast<StyleTransform>(transform),
      x.value_or(undefined),
      y.value_or(undefined),
      angle.value_or(undefined)
  };
}

std::vector<StyleTransformSpec> UnboxStyleTransformSpec(napi_env env, napi_value value) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::StyleTransformSpec)};

  if (!constructor) {
    if (napix::has_pending_exception(env)) {
      napi_value ignore;
      napi_get_and_clear_last_exception(env, &ignore);
    }
    return {};
  }

  auto transform{ UnboxStyleTransformSpec(env, value, constructor) };

  if (transform.has_value()) {
    return { transform.value() };
  }

  bool isArray{};

  if (napi_is_array(env, value, &isArray) == napi_ok && isArray) {
    uint32_t len{0};
    auto status{napi_get_array_length(env, value, &len)};

    if (status != napi_ok || len == 0) {
      return {};
    }

    std::vector<StyleTransformSpec> result;

    result.reserve(len);

    for (uint32_t i = 0; i < len; i++) {
      napi_value element{};
      napi_get_element(env, value, i, &element);

      transform = UnboxStyleTransformSpec(env, element, constructor);

      if (!transform) {
        return {};
      }

      result.push_back(transform.value());
    }

    return result;
  }

  return {};
}

static std17::optional<StyleValue> ParseStyleNumberString(const char* value) noexcept {
  if (strcmp(value, "auto") == 0) {
    return StyleValue::OfAuto();
  }

  try {
    return StyleValue::OfAnchor(StyleAnchorFromString(value));
  } catch (const std::invalid_argument&) {
    // exception from StyleAnchorFromString, assume value is not an anchor and continue parsing
  }

  char* end{};
  const auto parsed{ std::strtof(value, &end) };

  if (end && strcmp(end, value) != 0) {
    auto it{ sUnitMap.find(end) };

    if (it != sUnitMap.end()) {
      return StyleValue(parsed, it->second);
    }
  }

  return {};
}

static std17::optional<color_t> ParseHexHashColorString(const char* str) noexcept {
  static constexpr std::size_t kBufferSize{ 11 };
  char buffer[kBufferSize];

  switch (strlen(str)) {
    case 4: // #RGB
      snprintf(buffer, kBufferSize, "0xFF%c%c%c%c%c%c", str[1], str[1], str[2], str[2], str[3], str[3]);
      break;
    case 5: // #RGBA
      snprintf(buffer, kBufferSize, "0x%c%c%c%c%c%c%c%c",
               str[4], str[4], str[1], str[1], str[2], str[2], str[3], str[3]);
      break;
    case 7: // #RRGGBB
      snprintf(buffer, kBufferSize, "0xFF%s", &str[1]);
      break;
    case 9: // #RRGGBBAA
      snprintf(buffer, kBufferSize, "0x%c%c%c%c%c%c%c%c",
               str[7], str[8], str[1], str[2], str[3], str[4], str[5], str[6]);
      break;
    default:
      return {};
  }

  char* end;
  const auto color{ std::strtoul(buffer, &end, 16) };

  if (strcmp(end, str) == 0 || *end != '\0') {
    return {};
  } else {
    return { static_cast<uint32_t>(color & 0xFFFFFFFF) };
  }
}

napi_value StyleGetter(napi_env env, Style* style, StyleProperty property) {
  switch (StylePropertyMetaGetType(property)) {
    case StylePropertyMetaTypeEnum:
      return napix::to_value(env, style->GetEnumString(property));
    case StylePropertyMetaTypeColor:
      return BoxColor(env, style->GetColor(property));
    case StylePropertyMetaTypeString:
      if (style->IsEmpty(property)) {
        return {};
      } else {
        return napix::to_value(env, style->GetString(property));
      }
    case StylePropertyMetaTypeNumber:
      if (style->IsEmpty(property)) {
        return BoxStyleValue(env, StyleValue::OfUndefined());
      } else {
        return BoxStyleValue(env, style->GetNumber(property));
      }
    case StylePropertyMetaTypeTransform:
      if (style->IsEmpty(property)) {
        return BoxStyleTransformSpec(env, {});
      } else {
        return BoxStyleTransformSpec(env, style->GetTransform());
      }
    case StylePropertyMetaTypeInteger:
      if (style->IsEmpty(property)) {
        return {};
      } else {
        return napix::to_value(env, style->GetInteger(property).value());
      }
    default:
      assert(false);
      napix::throw_error(env, "style property has no type!");
      return {};
  }
}

static void StyleSetterEnum(napi_env env, Style* style, StyleProperty property, napi_value value) noexcept {
  if (napix::is_string(env, value)) {
    // 24 bytes is more than enough space for the longest style enum
    constexpr size_t kBufferSize{24};
    char buffer[kBufferSize];

    if (napi_get_value_string_utf8(env, value, buffer, kBufferSize, nullptr) == napi_ok) {
      // no utf8 encoded characters in valid style enums, so tolower is ok
      ToLowercase(buffer);

      if (StyleValidator::IsValidValue(property, buffer)) {
        style->SetEnum(property, buffer);
        return;
      }
    }
  }

  style->SetUndefined(property);
}

static void StyleSetterColor(napi_env env, Style* style, StyleProperty property, napi_value value) noexcept {
  auto color{ UnboxColor(env, value) };

  if (color.has_value()) {
    style->SetColor(property, color.value());
  } else {
    style->SetUndefined(property);
  }
}

static void StyleSetterNumber(napi_env env, Style* style, StyleProperty property, napi_value value) noexcept {
  auto number{ UnboxStyleValue(env, value) };

  if (number.has_value() && !number->IsUndefined() && StyleValidator::IsValidValue(property, number.value())) {
    style->SetNumber(property, number.value());
  } else {
    style->SetUndefined(property);
  }
}

static void StyleSetterInteger(napi_env env, Style* style, StyleProperty property, napi_value value) noexcept {
  if (napix::is_number(env, value)) {
    auto integer{ napix::as_int32(env, value, INT32_MAX) };

    if (StyleValidator::IsValidValue(property, integer)) {
      style->SetInteger(property, integer);
      return;
    }
  }

  style->SetUndefined(property);
}

void StyleSetter(napi_env env, Style* style, StyleProperty property, napi_value value) {
  switch (StylePropertyMetaGetType(property)) {
    case StylePropertyMetaTypeEnum:
      StyleSetterEnum(env, style, property, value);
      break;
    case StylePropertyMetaTypeString:
      if (napix::is_string(env, value)) {
        style->SetString(property, napix::as_string_utf8(env, value));
      } else {
        style->SetUndefined(property);
      }
      break;
    case StylePropertyMetaTypeColor:
      StyleSetterColor(env, style, property, value);
      break;
    case StylePropertyMetaTypeNumber: {
      StyleSetterNumber(env, style, property, value);
      break;
    }
    case StylePropertyMetaTypeInteger:
      StyleSetterInteger(env, style, property, value);
      break;
    case StylePropertyMetaTypeTransform:
      style->SetTransform(UnboxStyleTransformSpec(env, value));
      break;
    default:
      assert(false);
      napix::throw_error(env, "style property has no type!");
  }
}

} // namespace bindings
} // namespace lse
