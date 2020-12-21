/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/JSStyleClass.h>

#include <lse/Style.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsStyleClassConstructor;

namespace lse {
namespace bindings {

void JSStyleClass::Constructor(const CallbackInfo& info) {
  this->CreateNative();
}

Function JSStyleClass::GetClass(Napi::Env env) {
  if (jsStyleClassConstructor.IsEmpty()) {
    HandleScope scope(env);

#define LSE_PROP(ENUM) InstanceAccessor(                    \
            ToString(StyleProperty::ENUM),                         \
            [](JSStyleClass* instance, const CallbackInfo& info) { \
                return instance->Get(StyleProperty::ENUM, info);   \
            },                                                     \
            nullptr),

    jsStyleClassConstructor = DefineClass(env, "StyleClass", true, {
        InstanceMethod(Napi::SymbolFor(env, "set"), &JSStyleClass::Set),
        LSE_FOR_EACH_STYLE_PROPERTY(LSE_PROP)
    });

#undef LSE_PROP
  }

  return jsStyleClassConstructor.Value();
}

} // namespace bindings
} // namespace lse
