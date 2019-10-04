/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "StyleEnumMappings.h"
#include "StyleEnums.h"
#include <Yoga.h>

using Napi::Array;
using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

Value KeyValuePair(Napi::Env env, Napi::Value key, Napi::Value value) {
    auto pair = Array::New(env, 2);

    pair[0u] = key;
    pair[1u] = value;

    return pair;
}

template<typename E>
void AddMapping(Object target, const std::string& basePropertyName, const char* (*toString)(E)) {
    auto env{target.Env()};
    HandleScope scope{env};
    const auto enumCount{static_cast<uint32_t>(Count<E>())};
    auto fromStringPairs{Array::New(env, enumCount)};
    auto toStringPairs{Array::New(env, enumCount)};

    for (uint32_t i = 0; i < enumCount; i++) {
        const auto enumNumberValue{ Number::New(env, i) };
        const auto enumStringValue{ String::New(env, toString(static_cast<E>(i))) };

        fromStringPairs[i] = KeyValuePair(env, enumStringValue, enumNumberValue);
        toStringPairs[i] = KeyValuePair(env, enumNumberValue, enumStringValue);
    }

    const auto map{ env.Global().Get("Map").As<Function>() };

    target[basePropertyName + "ToString"] = map.New({toStringPairs});
    target[basePropertyName + "FromString"] = map.New({fromStringPairs});
}

Object StyleEnumMappings::Init(Env env, Object exports) {
    auto mappings = Object::New(env);

    AddMapping(mappings, "fontWeight", StyleFontWeightToString);
    AddMapping(mappings, "fontStyle", StyleFontStyleToString);
    AddMapping(mappings, "textTransform", StyleTextTransformToString);
    AddMapping(mappings, "backgroundClip", StyleBackgroundClipToString);
    AddMapping(mappings, "objectFit", StyleObjectFitToString);
    AddMapping(mappings, "textAlign", StyleTextAlignToString);
    AddMapping(mappings, "textOverflow", StyleTextOverflowToString);
    AddMapping(mappings, "anchor", StyleAnchorToString);

    AddMapping(mappings, "position", YGPositionTypeToString);
    AddMapping(mappings, "display", YGDisplayToString);
    AddMapping(mappings, "flexDirection", YGFlexDirectionToString);
    AddMapping(mappings, "flexWrap", YGWrapToString);
    AddMapping(mappings, "justify", YGJustifyToString);
    AddMapping(mappings, "overflow", YGOverflowToString);

    exports["StyleEnumMappings"] = mappings;

    return exports;
}

} // namespace ls
