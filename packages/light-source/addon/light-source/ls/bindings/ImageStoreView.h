/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {

class Scene;

namespace bindings {

class ImageStoreView : public Napi::SafeObjectWrap<ImageStoreView> {
 public:
    explicit ImageStoreView(const Napi::CallbackInfo& info);
    virtual ~ImageStoreView();

 private:
    void EnsureScene() const;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    void Add(const Napi::CallbackInfo& info);
    Napi::Value List(const Napi::CallbackInfo& info);
    Napi::Value GetExtensions(const Napi::CallbackInfo& info);
    void SetExtensions(const Napi::CallbackInfo& info, const Napi::Value& value);

 private:
    Scene* scene{};

    friend Napi::SafeObjectWrap<ImageStoreView>;
};

} // namespace bindings
} // namespace ls
