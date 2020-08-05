/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "GraphicsContext.h"

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Value;

#define CHECK_IMPL(impl) if (!(impl)) { throw Napi::Error::New(info.Env(), "PlatformPlugin is not initialized!"); }
#define CHECK_IMPL_STD(impl) if (!(impl)) { throw std::runtime_error("PlatformPlugin is not initialized!"); }

namespace ls {

GraphicsContext::GraphicsContext(const CallbackInfo& info) : Napi::SafeObjectWrap<GraphicsContext>(info) {
}

GraphicsContext::~GraphicsContext() {
    if (this->impl) {
        this->impl->Finalize();
    }
}

void GraphicsContext::Constructor(const CallbackInfo& info) {
    this->impl = Napi::ConstructorWithExternalFactory<GraphicsContextInterface, GraphicsContextInterfaceFactory>(
        info, "GraphicsContext");
}

Function GraphicsContext::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "GraphicsContext", true, {
            InstanceAccessor("width", &GraphicsContext::GetWidth, nullptr),
            InstanceAccessor("height", &GraphicsContext::GetHeight, nullptr),
            InstanceAccessor("displayIndex", &GraphicsContext::GetDisplayIndex, nullptr),
            InstanceAccessor("fullscreen", &GraphicsContext::GetFullscreen, nullptr),
            InstanceAccessor("title", &GraphicsContext::GetTitle, &GraphicsContext::SetTitle),
            InstanceMethod("attach", &GraphicsContext::Attach),
            InstanceMethod("detach", &GraphicsContext::Detach),
            InstanceMethod("resize", &GraphicsContext::Resize),
        });
    }

    return constructor.Value();
}

void GraphicsContext::Attach(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    this->impl->Attach(info);
}

void GraphicsContext::Detach(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    this->impl->Detach(info);
}

void GraphicsContext::Resize(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    this->impl->Resize(info);
}

Value GraphicsContext::GetTitle(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetTitle(info);
}

void GraphicsContext::SetTitle(const CallbackInfo& info, const Napi::Value& value) {
    CHECK_IMPL(this->impl);
    this->impl->SetTitle(info, value);
}

Value GraphicsContext::GetWidth(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetWidth(info);
}

Value GraphicsContext::GetHeight(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetHeight(info);
}

Value GraphicsContext::GetFullscreen(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetFullscreen(info);
}

Value GraphicsContext::GetDisplayIndex(const CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetDisplayIndex(info);
}

int32_t GraphicsContext::GetWidth() const {
    CHECK_IMPL_STD(this->impl);
    return this->impl->GetWidth();
}

int32_t GraphicsContext::GetHeight() const {
    CHECK_IMPL_STD(this->impl);
    return this->impl->GetHeight();
}

Renderer* GraphicsContext::GetRenderer() const {
    CHECK_IMPL_STD(this->impl);
    return this->impl->GetRenderer();
}

void GraphicsContext::Finalize() {
    throw std::runtime_error("Not implemented");
}

} // namespace ls

#undef CHECK_IMPL
#undef CHECK_IMPL_STD
