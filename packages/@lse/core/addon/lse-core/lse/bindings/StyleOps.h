/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/StyleEnums.h>
#include <lse/types.h>

namespace lse {
namespace bindings {

/**
 * API common between JSStyle and JSStyleClass.
 */
class StyleOps {
 public:
    virtual ~StyleOps() = default;

    StyleRef GetNative() const noexcept;

 protected:
    void CreateNative();
    void Set(const Napi::CallbackInfo& info);
    void Set(const Napi::Env& env, StyleProperty property, const Napi::Value& value);
    Napi::Value Get(StyleProperty property, const Napi::CallbackInfo& info);

 protected:
    StyleRef native;
};

} // namespace bindings
} // namespace lse
