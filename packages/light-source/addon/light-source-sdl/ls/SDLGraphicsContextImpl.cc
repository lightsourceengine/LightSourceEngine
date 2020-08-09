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
    HandleScope scope(info.Env());
    auto config{ info[1].As<Object>() };

    this->configDisplayIndex = Napi::ObjectGetNumberOrDefault(config, "displayIndex", 0);
    this->configWidth = Napi::ObjectGetNumberOrDefault(config, "width", 0);
    this->configHeight = Napi::ObjectGetNumberOrDefault(config, "height", 0);
    this->configFullscreen = Napi::ObjectGetBooleanOrDefault(config, "fullscreen", true);
}

void SDLGraphicsContextImpl::Attach(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };

    if (!this->window) {
        auto displayIndex{ this->configDisplayIndex };

        this->window = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            this->configWidth,
            this->configHeight,
            this->configFullscreen ? SDL_WINDOW_FULLSCREEN : 0);

        if (!this->window) {
            LOG_ERROR("SDL_CreateWindow(): %s", SDL_GetError());
            throw Error::New(env, "Failed to create window");
        }

        if (this->configFullscreen) {
            SDL_ShowCursor(SDL_DISABLE);
        }

        SDL_DisplayMode displayMode{};

        SDL_GetWindowDisplayMode(window, &displayMode);

        LOG_INFO("size=%i,%i format=%s refreshRate=%i displayIndex=%i",
            displayMode.w,
            displayMode.h,
            SDL_GetPixelFormatName(displayMode.format),
            displayMode.refresh_rate,
            displayIndex);
    }

    try {
        this->renderer.Attach(this->window);
    } catch (std::exception& e) {
        throw Error::New(env, e.what());
    }

    this->width = this->renderer.GetWidth();
    this->height = this->renderer.GetHeight();
    this->fullscreen = false;
}

void SDLGraphicsContextImpl::Detach(const Napi::CallbackInfo& info) {
    this->renderer.Detach();

    if (this->window) {
        SDL_DestroyWindow(this->window);
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
        SDL_SetWindowTitle(this->window, this->title.c_str());
    }
}

Renderer* SDLGraphicsContextImpl::GetRenderer() const {
    return &this->renderer;
}

void SDLGraphicsContextImpl::Finalize() {
    delete this;
}

} // namespace ls
