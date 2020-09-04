/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/types.h>

namespace ls {
namespace bindings {

/**
 * Bindings for a javascript Stage object.
 */
class JSStage : public Napi::SafeObjectWrap<JSStage> {
 public:
    using Napi::SafeObjectWrap<JSStage>::SafeObjectWrap;
    ~JSStage() override = default;

    static Napi::Function GetClass(Napi::Env env);
    StageRef GetNative() const noexcept;

 private:
    void Constructor(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info);

 private:
    StageRef native{};

    friend Napi::SafeObjectWrap<JSStage>;
};

} // namespace bindings
} // namespace ls
