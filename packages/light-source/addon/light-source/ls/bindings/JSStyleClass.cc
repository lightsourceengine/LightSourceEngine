/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/JSStyleClass.h>

#include <ls/Style.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsStyleClassConstructor;

namespace ls {
namespace bindings {

void JSStyleClass::Constructor(const CallbackInfo& info) {
     this->CreateNative();
}

Function JSStyleClass::GetClass(Napi::Env env) {
    if (jsStyleClassConstructor.IsEmpty()) {
        HandleScope scope(env);

        #define LS_PROP(ENUM) InstanceAccessor(                    \
            ToString(StyleProperty::ENUM),                         \
            [](JSStyleClass* instance, const CallbackInfo& info) { \
                return instance->Get(StyleProperty::ENUM, info);   \
            },                                                     \
            nullptr),

        jsStyleClassConstructor = DefineClass(env, "StyleClass", true, {
            InstanceMethod(Napi::SymbolFor(env, "set"), &JSStyleClass::Set),
            LS_FOR_EACH_STYLE_PROPERTY(LS_PROP)
        });

        #undef LS_PROP
    }

    return jsStyleClassConstructor.Value();
}

} // namespace bindings
} // namespace ls
