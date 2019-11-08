/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "StyleValue.h"
#include <ls/CStringHashMap.h>
#include <ls/Format.h>
#include <algorithm>
#include <cctype>

using Napi::Array;
using Napi::Error;
using Napi::Number;
using Napi::Object;
using Napi::String;

namespace ls {

static StyleValueNumber ParseNumber(const std::string& value);
static StyleValueColor ParseHexHashColor(const std::string& str);
static StyleValueNumber StyleValueNumberUnboxString(String value);
static StyleValueNumber StyleValueNumberUnboxArray(Array value);

static CStringHashMap<StyleNumberUnit> InitUnitMap();
static CStringHashMap<uint32_t> InitColorMap();

static CStringHashMap<StyleNumberUnit> sUnitMap = InitUnitMap();
static CStringHashMap<uint32_t> sColorMap = InitColorMap();

Napi::Value StyleValueColor::Box(Napi::Env env, const StyleValueColor& value) {
    if (value.undefined) {
        return env.Undefined();
    } else {
        return Napi::Number::New(env, value.value);
    }
}

StyleValueColor StyleValueColor::Unbox(const Napi::Value& value) {
    if (value.IsNumber()) {
        return StyleValueColor::Of(value.As<Napi::Number>().Uint32Value());
    } else if (value.IsString()) {
        std::string str{ value.As<String>() };

        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

        return StyleValueColor::Parse(str);
    } else {
        return StyleValueColor::OfUndefined();
    }
}

StyleValueColor StyleValueColor::Parse(const std::string& value) noexcept {
    if (!value.empty() && value[0] == '#') {
        return ParseHexHashColor(value);
    } else {
        auto it{ sColorMap.find(value.c_str()) };

        if (it != sColorMap.end()) {
            return StyleValueColor::Of(it->second);
        } else {
            return StyleValueColor::OfUndefined();
        }
    }
}

Napi::Value StyleValueNumber::Box(Napi::Env env, const StyleValueNumber& value) {
    auto array{ Array::New(env, 2) };

    switch (value.unit) {
        case StyleNumberUnitUndefined:
            array[0u] = env.Undefined();
            break;
        case StyleNumberUnitAuto:
            array[0u] = String::New(env, "auto");
            break;
        case StyleNumberUnitAnchor:
            array[0u] = String::New(env, StyleAnchorToString(static_cast<StyleAnchor>(value.AsInt32())));
            break;
        default:
            array[0u] = Number::New(env, value.value);
            break;
    }

    array[1u] = Number::New(env, value.unit);

    return array;
}

StyleValueNumber StyleValueNumber::Unbox(const Napi::Value& value) {
    if (value.IsNumber()) {
        const float floatValue{value.As<Number>()};

        if (std::isnan(floatValue)) {
            return {};
        } else {
            return StyleValueNumber::OfPoint(floatValue);
        }
    } else if (value.IsArray()) {
        return StyleValueNumberUnboxArray(value.As<Array>());
    } else if (value.IsString()) {
        return StyleValueNumberUnboxString(value.As<String>());
    } else {
        return {};
    }
}

Napi::Value StyleValueString::Box(Napi::Env env, const std::string& value) {
    return Napi::String::New(env, value);
}

std::string StyleValueString::Unbox(const Napi::Value& value) {
    if (value.IsString()) {
        return value.As<Napi::String>();
    } else {
        return "";
    }
}

static StyleValueNumber ParseNumber(const std::string& value) {
    char* end{};
    const auto str{ value.c_str() };
    const auto parsed{ std::strtof(str, &end) };

    if (end && end != str) {
        auto it{sUnitMap.find(end) };

        if (it != sUnitMap.end()) {
            return StyleValueNumber::Of(parsed, it->second);
        }
    }

    return StyleValueNumber::OfUndefined();
}

static StyleValueColor ParseHexHashColor(const std::string& str) {
    constexpr std::size_t BUFFER_SIZE{11};
    char buffer[BUFFER_SIZE];

    switch (str.size()) {
        case 4: // #RGB
            snprintf(buffer, BUFFER_SIZE, "0xFF%c%c%c%c%c%c", str[1], str[1], str[2], str[2], str[3], str[3]);
            break;
        case 5: // #ARGB
            snprintf(buffer, BUFFER_SIZE, "0x%c%c%c%c%c%c%c%c",
                str[1], str[1], str[2], str[2], str[3], str[3], str[4], str[4]);
            break;
        case 7: // #RRGGBB
            snprintf(buffer, BUFFER_SIZE, "0xFF%s", &str[1]);
            break;
        case 9: // #AARRGGBB
            snprintf(buffer, BUFFER_SIZE, "0x%s", &str[1]);
            break;
        default:
            *buffer = '\0';
            break;
    }

    char* end;
    const auto color{ std::strtoul(buffer, &end, 16) };

    if (end == str || *end != '\0') {
        return StyleValueColor::OfUndefined();
    } else {
        return StyleValueColor::Of(static_cast<uint32_t>(color & 0xFFFFFFFF));
    }
}

static StyleValueNumber StyleValueNumberUnboxString(String value) {
    const std::string str{ value.As<String>() };

    if (str == "auto") {
        return StyleValueNumber::OfAuto();
    }

    try {
        return StyleValueNumber::Of(StyleAnchorFromString(str.c_str()), StyleNumberUnitAnchor);
    } catch (const std::invalid_argument&) {
    }

    return ParseNumber(str);
}

static StyleValueNumber StyleValueNumberUnboxArray(Array value) {
    auto second{ value.Get(1u) };
    int32_t unitInt{-1};

    if (second.IsNumber()) {
        unitInt = second.As<Number>();
    }

    if (!IsEnum<StyleNumberUnit>(unitInt)) {
        return StyleValueNumber::OfUndefined();
    }

    auto unit{static_cast<StyleNumberUnit>(unitInt) };

    switch (unit) {
        case StyleNumberUnitAuto:
            return StyleValueNumber::OfAuto();
        case StyleNumberUnitAnchor: {
            auto first{ value.Get(0u) };

            if (first.IsString()) {
                std::string str{ first.As<String>() };

                try {
                    return StyleValueNumber::OfAnchor(StyleAnchorFromString(str.c_str()));
                } catch (const std::invalid_argument&) {
                }
            }

            return StyleValueNumber::OfUndefined();
        }
        case StyleNumberUnitUndefined:
            return StyleValueNumber::OfUndefined();
        default: {
            auto first{ value.Get(0u) };

            if (first.IsNumber()) {
                return StyleValueNumber::Of(first.As<Number>(), unit);
            } else {
                return StyleValueNumber::OfUndefined();
            }
        }
    }
}

static CStringHashMap<StyleNumberUnit> InitUnitMap() {
    return {
        {"px", StyleNumberUnitPoint },
        {"%", StyleNumberUnitPercent },
        {"vw", StyleNumberUnitViewportWidth },
        {"vh", StyleNumberUnitViewportHeight },
        {"vmin", StyleNumberUnitViewportMin },
        {"vmax", StyleNumberUnitViewportMax },
        {"rem", StyleNumberUnitRootEm }
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

} // namespace ls
