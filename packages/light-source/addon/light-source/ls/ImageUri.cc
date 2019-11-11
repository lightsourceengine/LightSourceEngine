/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageUri.h"
#include <napi-ext.h>
#include <algorithm>

using Napi::EscapableHandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectGetNumberOrDefault;
using Napi::ObjectGetString;
using Napi::ObjectGetStringOrEmpty;
using Napi::String;
using Napi::Value;

namespace ls {

EdgeRect UnboxCapInsets(const Value& value);

ImageUri::ImageUri(const std::string& uri) noexcept : uri(uri) {
}

ImageUri::ImageUri(const std::string& uri, const std::string& id, const int32_t width, const int32_t height) noexcept
: uri(uri), id(id), width(width), height(height) {
}

ImageUri::ImageUri(
    const std::string& uri,
    const std::string& id,
    const int32_t width,
    const int32_t height,
    const EdgeRect& capInsets) noexcept
: uri(uri), id(id), width(width), height(height), capInsets(capInsets),
      hasCapInsets(capInsets.top || capInsets.right || capInsets.bottom || capInsets.left) {
}

Napi::Value ImageUri::Box(Napi::Env env, const ImageUri& value) {
    auto result{ Object::New(env) };

    result["id"] = String::New(env, value.GetId());
    result["uri"] = String::New(env, value.GetUri());

    if (value.width > 0) {
        result["width"] = Number::New(env, value.width);
    }

    if (value.height > 0) {
        result["height"] = Number::New(env, value.height);
    }

    if (value.HasCapInsets()) {
        auto capInsets{ Object::New(env) };

        capInsets["top"] = Number::New(env, value.capInsets.top);
        capInsets["right"] = Number::New(env, value.capInsets.right);
        capInsets["bottom"] = Number::New(env, value.capInsets.bottom);
        capInsets["left"] = Number::New(env, value.capInsets.left);

        result["capInsets"] = capInsets;
    }

    return result;
}

ImageUri ImageUri::Unbox(const Napi::Value& value) {
    if (value.IsString()) {
        return {
            ImageUri(value.As<String>())
        };
    }

    auto spec{ value.As<Object>() };

    return {
        ObjectGetStringOrEmpty(spec, "uri"),
        ObjectGetStringOrEmpty(spec, "id"),
        std::max(ObjectGetNumberOrDefault(spec, "width", 0), 0),
        std::max(ObjectGetNumberOrDefault(spec, "height", 0), 0),
        UnboxCapInsets(spec.Get("capInsets"))
    };
}

EdgeRect UnboxCapInsets(const Value& value) {
    if (value.IsNumber()) {
        const auto component{ std::max(value.As<Number>().Int32Value(), 0) };

        return {
            component,
            component,
            component,
            component
        };
    }

    if (!value.IsObject()) {
        return {};
    }

    auto spec{ value.As<Object>() };

    return {
        std::max(ObjectGetNumberOrDefault<int32_t>(spec, "top", 0), 0),
        std::max(ObjectGetNumberOrDefault<int32_t>(spec, "right", 0), 0),
        std::max(ObjectGetNumberOrDefault<int32_t>(spec, "bottom", 0), 0),
        std::max(ObjectGetNumberOrDefault<int32_t>(spec, "left", 0), 0),
    };
}

} // namespace ls
