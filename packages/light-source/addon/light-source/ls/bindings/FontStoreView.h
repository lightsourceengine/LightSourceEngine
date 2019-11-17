/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/StyleEnums.h>

namespace ls {

class Stage;

namespace bindings {

class FontStoreView : public Napi::SafeObjectWrap<FontStoreView> {
 public:
    explicit FontStoreView(const Napi::CallbackInfo& info);
    virtual ~FontStoreView();

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    void Add(const Napi::CallbackInfo& info);
    void Remove(const Napi::CallbackInfo& info);
    Napi::Value List(const Napi::CallbackInfo& info);

 private:
    void EnsureStage() const;
    void AddFont(const std::string& family, StyleFontStyle style, StyleFontWeight weight,
                 const std::string& uri, int32_t ttfIndex);
    void AddFontFamily(const std::string& familyUri);

 private:
    Stage* stage{};

    friend SafeObjectWrap<FontStoreView>;
};

} // namespace bindings
} // namespace ls
