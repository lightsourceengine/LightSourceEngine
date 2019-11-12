/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class Scene;

namespace bindings {

class ImageStoreView : public Napi::ObjectWrap<ImageStoreView> {
 public:
    explicit ImageStoreView(const Napi::CallbackInfo& info);
    virtual ~ImageStoreView();

    static Napi::Function Constructor(Napi::Env env);

 private:
    void Construct(const Napi::CallbackInfo& info);
    void EnsureScene() const;

 private: // javascript bindings
    void Add(const Napi::CallbackInfo& info);
    Napi::Value List(const Napi::CallbackInfo& info);
    Napi::Value GetExtensions(const Napi::CallbackInfo& info);
    void SetExtensions(const Napi::CallbackInfo& info, const Napi::Value& value);

 private:
    Scene* scene{};
};

} // namespace bindings
} // namespace ls
