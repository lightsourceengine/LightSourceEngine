/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStoreView.h"
#include "FontStore.h"
#include "Stage.h"
#include <fmt/format.h>
#include <napi-ext.h>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectGetString;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

StyleFontStyle StringToFontStyle(Napi::Env env, const std::string& value, bool isRequired);
StyleFontWeight StringToFontWeight(Napi::Env env, const std::string& value, bool isRequired);

FontStoreView::FontStoreView(const CallbackInfo& info) : ObjectWrap<FontStoreView>(info) {
    auto env{ info.Env() };

    if (info[0].IsObject()) {
        this->stage = Stage::Unwrap(info[0].As<Object>());
    }

    if (stage == nullptr) {
        throw Error::New(env, "FontStoreView expects a Stage instance.");
    }

    this->stage->Ref();
}

FontStoreView::~FontStoreView() {
    this->stage->Unref();
}

Function FontStoreView::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "FontStoreView", {
            InstanceMethod("add", &FontStoreView::Add),
            InstanceMethod("remove", &FontStoreView::Remove),
            InstanceMethod("all", &FontStoreView::All),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void FontStoreView::Add(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (!info[0].IsObject()) {
        throw Error::New(env, "add() expects an Object with font creation options");
    }

    auto options{ info[0].As<Object>() };

    FontId fontId {
        ObjectGetString(options, "family"),
        StringToFontStyle(env, ObjectGetStringOrEmpty(options, "style"), false),
        StringToFontWeight(env, ObjectGetStringOrEmpty(options, "weight"), false)
    };

    if (this->stage->GetFontStore()->Has(fontId)) {
        throw Error::New(env, fmt::format("font (family='{}' style='{}' weight='{}') already exists",
            fontId.family, StyleFontStyleToString(fontId.style), StyleFontWeightToString(fontId.weight)));
    }

    auto font = std::make_shared<FontResource>(
        fontId,
        ObjectGetString(options, "uri"),
        ObjectGetNumberOrDefault(options, "index", 0));

    font->Load(this->stage);

    this->stage->GetFontStore()->Add(font);
}

void FontStoreView::Remove(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (!info[0].IsObject()) {
        throw Error::New(env, "remove() expects an Object with family, style and weight properties");
    }

    auto options{ info[0].As<Object>() };

    this->stage->GetFontStore()->Remove({
        ObjectGetString(options, "family"),
        StringToFontStyle(env, ObjectGetString(options, "style"), true),
        StringToFontWeight(env, ObjectGetString(options, "weight"), true)
    });
}

Value FontStoreView::All(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto fontList{ Array::New(env) };
    auto fontStore{ this->stage->GetFontStore() };

    fontStore->ForEach([&](std::shared_ptr<FontResource> resource) {
        HandleScope scope(env);
        auto font{ Object::New(env) };

        font["family"] = String::New(env, resource->GetFontFamily());
        font["style"] = String::New(env, StyleFontStyleToString(resource->GetFontStyle()));
        font["weight"] = String::New(env, StyleFontWeightToString(resource->GetFontWeight()));
        font["uri"] = String::New(env, resource->GetUri());
        font["index"] = Number::New(env, resource->GetIndex());
        font["state"] = String::New(env, ResourceStateToString(resource->GetState()));
        font["refs"] = Number::New(env, resource.use_count() - 1);

        fontList[fontList.Length()] = font;
    });

    return fontList;
}

StyleFontStyle StringToFontStyle(Napi::Env env, const std::string& value, bool isRequired) {
    if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
        return StyleFontStyleNormal;
    } else if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
        return StyleFontStyleItalic;
    } else if (isRequired) {
        throw Error::New(env, fmt::format("Invalid font style string: {}", value));
    }

    return StyleFontStyleNormal;
}

StyleFontWeight StringToFontWeight(Napi::Env env, const std::string& value, bool isRequired) {
    if (value == StyleFontWeightToString(StyleFontWeightNormal)) {
        return StyleFontWeightNormal;
    } else if (value == StyleFontWeightToString(StyleFontWeightBold)) {
        return StyleFontWeightBold;
    } else if (isRequired) {
        throw Error::New(env, fmt::format("Invalid font weight string: {}", value));
    }

    return StyleFontWeightNormal;
}

} // namespace ls
