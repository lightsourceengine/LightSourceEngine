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
 * Bindings for a javascript StyleClass object.
 */
class JSStyleClass : public Napi::SafeObjectWrap<JSStyleClass>, public StyleOps {
 public:
  using Napi::SafeObjectWrap<JSStyleClass>::SafeObjectWrap;
  ~JSStyleClass() override = default;

  static Napi::Function GetClass(Napi::Env env);

 private:
  void Constructor(const Napi::CallbackInfo& info) override;

  friend Napi::SafeObjectWrap<JSStyleClass>;
};

} // namespace bindings
} // namespace lse
