/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/bindings/StyleOps.h>

namespace lse {
namespace bindings {

/**
 * Bindings for a javascript Style object.
 */
class JSStyle : public Napi::SafeObjectWrap<JSStyle>, public StyleOps {
 public:
  using Napi::SafeObjectWrap<JSStyle>::SafeObjectWrap;
  ~JSStyle() override = default;

  static Napi::Function GetClass(Napi::Env env);

 private:
  void Constructor(const Napi::CallbackInfo& info) override;
  void Reset(const Napi::CallbackInfo& info);
  Napi::Value BindStyleClass(const Napi::CallbackInfo& info);

 private:
  friend Napi::SafeObjectWrap<JSStyle>;
};

} // namespace bindings
} // namespace lse
