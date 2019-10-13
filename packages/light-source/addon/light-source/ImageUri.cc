/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageUri.h"
#include <napi-ext.h>

using Napi::EscapableHandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectGetNumberOrDefault;
using Napi::ObjectGetString;
using Napi::ObjectGetStringOrEmpty;
using Napi::String;
using Napi::Value;

namespace ls {

EdgeRect ToCapInsets(const Object& spec) noexcept {
    return {
        ObjectGetNumberOrDefault(spec, "top", 0),
        ObjectGetNumberOrDefault(spec, "right", 0),
        ObjectGetNumberOrDefault(spec, "bottom", 0),
        ObjectGetNumberOrDefault(spec, "left", 0),
    };
}

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

ImageUri ImageUri::FromObject(const Object& spec) {
    const auto uri{ ObjectGetString(spec, "uri") };
    const auto id{ ObjectGetStringOrEmpty(spec, "id") };
    const auto width{ std::max(ObjectGetNumberOrDefault(spec, "width", 0), 0) };
    const auto height{ std::max(ObjectGetNumberOrDefault(spec, "height", 0), 0) };

    if (spec.Has("capInsets") && spec.Get("capInsets").IsObject()) {
        return ImageUri(uri, id, width, height, ToCapInsets(spec.Get("capInsets").As<Object>()));
    }

    return ImageUri(uri, id, width, height);
}

Value ImageUri::ToObject(const Napi::Env& env) const {
    EscapableHandleScope scope(env);
    auto imageUri{ Object::New(env) };

    imageUri["id"] = String::New(env, this->GetId());
    imageUri["uri"] = String::New(env, this->GetUri());

    if (this->width > 0) {
        imageUri["width"] = Number::New(env, this->width);
    }

    if (this->height > 0) {
        imageUri["height"] = Number::New(env, this->height);
    }

    if (this->HasCapInsets()) {
        auto capInsets{ Object::New(env) };

        capInsets["top"] = Number::New(env, this->capInsets.top);
        capInsets["right"] = Number::New(env, this->capInsets.right);
        capInsets["bottom"] = Number::New(env, this->capInsets.bottom);
        capInsets["left"] = Number::New(env, this->capInsets.left);

        imageUri["capInsets"] = capInsets;
    }

    return scope.Escape(imageUri);
}

} // namespace ls
