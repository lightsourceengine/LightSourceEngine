/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStoreView.h"
#include "FontStore.h"
#include "Stage.h"
#include <ls/Format.h>
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

static void EnsureFontStoreAttached(Stage* stage);
static StyleFontStyle StringToFontStyle(const Napi::Env& env, const std::string& value, const bool isRequired);
static StyleFontWeight StringToFontWeight(const Napi::Env& env, const std::string& value, const bool isRequired);

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
            InstanceMethod("list", &FontStoreView::List),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void FontStoreView::Add(const CallbackInfo& info) {
    EnsureFontStoreAttached(this->stage);

    auto env{ info.Env() };
    HandleScope scope(env);

    if (!info[0].IsObject()) {
        throw Error::New(env, "add() expects an Object with font creation options");
    }

    const auto options{ info[0].As<Object>() };

    const FontId fontId {
        ObjectGetString(options, "family"),
        StringToFontStyle(env, ObjectGetStringOrEmpty(options, "style"), false),
        StringToFontWeight(env, ObjectGetStringOrEmpty(options, "weight"), false)
    };

    if (this->stage->GetFontStore()->Has(fontId)) {
        throw Error::New(env, Format("%s already exists", static_cast<std::string>(fontId)));
    }

    try {
        this->stage->GetFontStore()->AddFont(
            fontId,
            ObjectGetString(options, "uri"),
            ObjectGetNumberOrDefault(options, "index", 0));
    } catch (const std::exception& e) {
        throw Error::New(env, Format("Error adding font: %s", e.what()));
    }
}

void FontStoreView::Remove(const CallbackInfo& info) {
    EnsureFontStoreAttached(this->stage);

    auto env{ info.Env() };
    HandleScope scope(env);

    if (!info[0].IsObject()) {
        throw Error::New(env, "remove() expects an Object with family, style and weight properties");
    }

    auto options{ info[0].As<Object>() };

    this->stage->GetFontStore()->RemoveFont({
        ObjectGetString(options, "family"),
        StringToFontStyle(env, ObjectGetString(options, "style"), true),
        StringToFontWeight(env, ObjectGetString(options, "weight"), true)
    });
}

Value FontStoreView::List(const CallbackInfo& info) {
    EnsureFontStoreAttached(this->stage);

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

static void EnsureFontStoreAttached(Stage* stage) {
    if (!stage->GetFontStore()->IsAttached()) {
        throw Napi::Error::New(stage->Env(), "Cannot use FontStoreView after Stage has been destroyed.");
    }
}

static StyleFontStyle StringToFontStyle(const Napi::Env& env, const std::string& value, const bool isRequired) {
    if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
        return StyleFontStyleNormal;
    } else if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
        return StyleFontStyleItalic;
    } else if (isRequired) {
        throw Error::New(env, Format("Invalid font style string: %s", value));
    }

    return StyleFontStyleNormal;
}

static StyleFontWeight StringToFontWeight(const Napi::Env& env, const std::string& value, const bool isRequired) {
    if (value == StyleFontWeightToString(StyleFontWeightNormal)) {
        return StyleFontWeightNormal;
    } else if (value == StyleFontWeightToString(StyleFontWeightBold)) {
        return StyleFontWeightBold;
    } else if (isRequired) {
        throw Error::New(env, Format("Invalid font weight string: %s", value));
    }

    return StyleFontWeightNormal;
}

} // namespace ls
