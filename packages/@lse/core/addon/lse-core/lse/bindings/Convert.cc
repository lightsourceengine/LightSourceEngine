/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/Convert.h>

#include <lse/bindings/JSStyleValue.h>
#include <lse/string-ext.h>
#include <lse/CStringHashMap.h>

using Napi::Number;
using Napi::Object;
using Napi::String;

namespace lse {
namespace bindings {

static CStringHashMap<StyleNumberUnit> InitUnitMap();
static CStringHashMap<uint32_t> InitColorMap();
static std17::optional<color_t> ParseHexHashColorString(const char* str) noexcept;
static std17::optional<StyleValue> ParseStyleNumberString(const char* value) noexcept;

static CStringHashMap<StyleNumberUnit> sUnitMap = InitUnitMap();
static CStringHashMap<uint32_t> sColorMap = InitColorMap();

Napi::Value BoxColor(const Napi::Env& env, const std17::optional<color_t>& color) {
    if (color.has_value()) {
        return Napi::Number::New(env, color->value);
    } else {
        return env.Undefined();
    }
}

std17::optional<color_t> UnboxColor(const Napi::Env& env, const Napi::Value& value) {
    switch (value.Type()) {
        case napi_string: {
            // 256 bytes is more than enough space for the longest css color name and # colors
            char* str = Napi::CopyUtf8(value);

            if (str[0] == '#') {
                return ParseHexHashColorString(ToLowercase(str));
            } else {
                auto it{ sColorMap.find(ToLowercase(str)) };

                if (it != sColorMap.end()) {
                    return { it->second };
                }

                return {};
            }
        }
        case napi_number:
            return { value.As<Number>().Uint32Value() };
        default:
            return {};
    }
}

std17::optional<StyleValue> UnboxStyleValue(const Napi::Env& env, const Napi::Value& value) {
    switch (value.Type()) {
        case napi_string:
            return ParseStyleNumberString(ToLowercase(Napi::CopyUtf8(value)));
        case napi_number: {
            const float floatValue{value.As<Number>()};

            if (std::isnan(floatValue)) {
                return {};
            }

            return StyleValue::OfPoint(floatValue);
        }
        default: {
            auto styleValue = JSStyleValue::ToStyleValue(value);

            if (styleValue.IsUndefined()) {
                return {};
            }

            return styleValue;
        }
    }
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
            return {{ it->second, parsed }};
        }
    }

    return {};
}

static std17::optional<color_t> ParseHexHashColorString(const char* str) noexcept {
    static const std::size_t bufferSize{11};
    static char buffer[bufferSize];

    switch (strlen(str)) {
        case 4: // #RGB
            snprintf(buffer, bufferSize, "0xFF%c%c%c%c%c%c", str[1], str[1], str[2], str[2], str[3], str[3]);
            break;
        case 5: // #RGBA
            snprintf(buffer, bufferSize, "0x%c%c%c%c%c%c%c%c",
                     str[4], str[4], str[1], str[1], str[2], str[2], str[3], str[3]);
            break;
        case 7: // #RRGGBB
            snprintf(buffer, bufferSize, "0xFF%s", &str[1]);
            break;
        case 9: // #RRGGBBAA
            snprintf(buffer, bufferSize, "0x%c%c%c%c%c%c%c%c",
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

static CStringHashMap<StyleNumberUnit> InitUnitMap() {
    return {
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
}

static CStringHashMap<uint32_t> InitColorMap() {
    return {
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
}

} // namespace bindings
} // namespace lse
