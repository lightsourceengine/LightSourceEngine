/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/SDLGraphicsContextImpl.h>

#include <ls/Log.h>
#include <napi-ext.h>

using Napi::Boolean;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

SDLGraphicsContextImpl::SDLGraphicsContextImpl(const Napi::CallbackInfo& info) {
    auto config{ info[1].As<Object>() };

    this->configDisplayIndex = Napi::ObjectGetNumberOrDefault(config, "displayIndex", 0);
    this->configWidth = Napi::ObjectGetNumberOrDefault(config, "width", 0);
    this->configHeight = Napi::ObjectGetNumberOrDefault(config, "height", 0);
    this->configFullscreen = Napi::ObjectGetBooleanOrDefault(config, "fullscreen", true);
}

void SDLGraphicsContextImpl::Attach(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    bool isWindowFullscreen{ false };

    if (!this->window) {
        auto displayIndex{ this->configDisplayIndex };

        this->window = SDL2::SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            this->configWidth,
            this->configHeight,
            this->configFullscreen ? SDL_WINDOW_FULLSCREEN : 0);

        if (!this->window) {
            LOG_ERROR("SDL_CreateWindow(): %s", SDL2::SDL_GetError());
            throw Error::New(env, "Failed to create window");
        }

        isWindowFullscreen = (SDL2::SDL_GetWindowFlags(this->window)
            & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP));

        if (isWindowFullscreen) {
            // TODO: if mouse events are ever supported, this should be configurable.
            SDL2::SDL_ShowCursor(SDL_DISABLE);
        }

        SDL_DisplayMode displayMode{};

        SDL2::SDL_GetWindowDisplayMode(this->window, &displayMode);

        LOGX_INFO("SDL_Window: %ix%i @ %ihz, format=%s, fullscreen=%s",
            displayMode.w,
            displayMode.h,
            displayMode.refresh_rate,
            SDL2::SDL_GetPixelFormatName(displayMode.format),
            isWindowFullscreen);
    }

    NAPI_TRY(env, this->renderer.Attach(this->window));

    this->width = this->renderer.GetWidth();
    this->height = this->renderer.GetHeight();
    this->fullscreen = isWindowFullscreen;
}

void SDLGraphicsContextImpl::Detach(const Napi::CallbackInfo& info) {
    this->renderer.Detach();

    if (this->window) {
        SDL2::SDL_DestroyWindow(this->window);
        this->window = nullptr;
    }
}

void SDLGraphicsContextImpl::Resize(const Napi::CallbackInfo& info) {
    // TODO: int32_t width, int32_t height, bool fullscreen
}

Value SDLGraphicsContextImpl::GetWidth(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->width);
}

Value SDLGraphicsContextImpl::GetHeight(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->height);
}

Value SDLGraphicsContextImpl::GetDisplayIndex(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->configDisplayIndex);
}

Value SDLGraphicsContextImpl::GetFullscreen(const Napi::CallbackInfo& info) {
    return Boolean::New(info.Env(), this->fullscreen);
}

Value SDLGraphicsContextImpl::GetTitle(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), this->title);
}

void SDLGraphicsContextImpl::SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->title = value.As<String>();
    } else {
        throw Error::New(info.Env(), "title property must be a string");
    }

    if (this->window) {
        SDL2::SDL_SetWindowTitle(this->window, this->title.c_str());
    }
}

Renderer* SDLGraphicsContextImpl::GetRenderer() const {
    return &this->renderer;
}

void SDLGraphicsContextImpl::Finalize() {
    delete this;
}

} // namespace ls
