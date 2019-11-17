/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStoreView.h"
#include <ls/FontStore.h>
#include <ls/Stage.h>
#include <ls/FileSystem.h>
#include <ls/Format.h>
#include <ls/Log.h>
#include <std17/filesystem>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectGetString;
using Napi::SafeObjectWrap;
using Napi::QueryInterface;
using Napi::String;
using Napi::Value;

namespace ls {
namespace bindings {

static StyleFontStyle StringToFontStyle(const Napi::Env& env, const std::string& value, const bool isRequired);
static StyleFontWeight StringToFontWeight(const Napi::Env& env, const std::string& value, const bool isRequired);

FontStoreView::FontStoreView(const CallbackInfo& info) : SafeObjectWrap<FontStoreView>(info) {
}

FontStoreView::~FontStoreView() {
    if (this->stage) {
        this->stage->Unref();
    }
}

void FontStoreView::Constructor(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (info[0].IsObject()) {
        this->stage = QueryInterface<Stage>(info[0]);
        if (this->stage) {
            this->stage->Ref();
        }
    }
}

void FontStoreView::EnsureStage() const {
    if (this->stage == nullptr || !stage->GetFontStore()->IsAttached()) {
        throw Error::New(this->Env(), "FontStoreView not connected to Stage.");
    }
}

Function FontStoreView::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "FontStoreView", {
            InstanceMethod("add", &FontStoreView::Add),
            InstanceMethod("remove", &FontStoreView::Remove),
            InstanceMethod("list", &FontStoreView::List),
        });
    }

    return constructor.Value();
}

void FontStoreView::Add(const CallbackInfo& info) {
    this->EnsureStage();

    auto env{ info.Env() };
    HandleScope scope(env);
    auto arg{ info[0] };

    if (arg.IsString()) {
        std17::filesystem::path p{ arg.As<String>() };

        if (p.has_extension()) {
            this->AddFont(p.stem(), StyleFontStyleNormal, StyleFontWeightNormal, p.native(), 0);
        } else {
            this->AddFontFamily(p.native());
        }
    } else if (arg.IsObject()) {
        const auto options{ arg.As<Object>() };

        this->AddFont(
            ObjectGetString(options, "family"),
            StringToFontStyle(env, ObjectGetStringOrEmpty(options, "style"), false),
            StringToFontWeight(env, ObjectGetStringOrEmpty(options, "weight"), false),
            ObjectGetString(options, "uri"),
            ObjectGetNumberOrDefault(options, "index", 0));
    } else {
        throw Error::New(env, "add() expects a String uri or Object with font creation options");
    }
}

void FontStoreView::Remove(const CallbackInfo& info) {
    this->EnsureStage();

    auto env{ info.Env() };
    HandleScope scope(env);

    if (!info[0].IsObject()) {
        throw Error::New(env, "remove() expects an Object with family, style and weight properties");
    }

    auto options{ info[0].As<Object>() };

    this->stage->GetFontStore()->RemoveFont(
        ObjectGetString(options, "family"),
        StringToFontStyle(env, ObjectGetString(options, "style"), true),
        StringToFontWeight(env, ObjectGetString(options, "weight"), true));
}

Value FontStoreView::List(const CallbackInfo& info) {
    this->EnsureStage();

    auto env{ info.Env() };
    auto fontList{ Array::New(env) };
    auto fontStore{ this->stage->GetFontStore() };

    fontStore->ForEach([&](const std::shared_ptr<FontResource>& resource) {
        HandleScope scope(env);
        auto font{ Object::New(env) };

        font["family"] = String::New(env, resource->GetFontFamily());
        font["style"] = String::New(env, StyleFontStyleToString(resource->GetFontStyle()));
        font["weight"] = String::New(env, StyleFontWeightToString(resource->GetFontWeight()));
        font["uri"] = String::New(env, resource->GetUri());
        font["index"] = Number::New(env, resource->GetIndex());
        font["state"] = String::New(env, ResourceStateToString(resource->GetState()));
        font["refs"] = Number::New(env, static_cast<int32_t>(resource.use_count() - 1));

        fontList[fontList.Length()] = font;
    });

    return fontList;
}
void FontStoreView::AddFont(const std::string &family, StyleFontStyle style, StyleFontWeight weight,
                            const std::string &uri, int32_t ttfIndex) {
    try {
        this->stage->GetFontStore()->AddFont(family, style, weight, uri, ttfIndex);
    } catch (const std::exception& e) {
        throw Error::New(this->Env(), Format("Font: %s (%s, %s) Error: %s",
            family, StyleFontStyleToString(style), StyleFontWeightToString(weight), e.what()));
    }
}

void FontStoreView::AddFontFamily(const std::string& familyUri) {
    std17::filesystem::path filename;

    if (IsResourceUri(familyUri)) {
        filename = this->stage->GetResourcePath() + GetResourceUriPath(familyUri);
    } else {
        filename = familyUri;
    }

    if (std17::filesystem::exists(filename.native() + "-Regular" + ".ttf")) {
        this->AddFont(filename.stem(), StyleFontStyleNormal, StyleFontWeightNormal,
            familyUri + "-Regular" + ".ttf", 0);
    } else if (std17::filesystem::exists(filename.native() + ".ttf")) {
        this->AddFont(filename.stem(), StyleFontStyleNormal, StyleFontWeightNormal,
            familyUri + ".ttf", 0);
    }

    if (std17::filesystem::exists(filename.native() + "-Bold" + ".ttf")) {
        this->AddFont(filename.stem(), StyleFontStyleNormal, StyleFontWeightBold,
            familyUri + "-Bold" + ".ttf", 0);
    }

    if (std17::filesystem::exists(filename.native() + "-Italic" + ".ttf")) {
        this->AddFont(filename.stem(), StyleFontStyleItalic, StyleFontWeightNormal,
            familyUri + "-Italic" + ".ttf", 0);
    }

    if (std17::filesystem::exists(filename.native() + "-BoldItalic" + ".ttf")) {
        this->AddFont(filename.stem(), StyleFontStyleItalic, StyleFontWeightBold,
            familyUri + "-BoldItalic" + ".ttf", 0);
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

} // namespace bindings
} // namespace ls
