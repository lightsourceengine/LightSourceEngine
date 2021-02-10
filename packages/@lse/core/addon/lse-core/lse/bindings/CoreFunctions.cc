/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/CoreFunctions.h>

#include <napix.h>
#include <lse/Habitat.h>
#include <lse/bindings/CStyleUtil.h>
#include <lse/AudioPlugin.h>
#include <lse/SDLAudioPluginImpl.h>
#include <lse/SDLMixerAudioPluginImpl.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/Config.h>
#include <lse/bindings/SDLPlatformPluginExports.h>
#include <lse/bindings/CoreClasses.h>

namespace lse {
namespace bindings {

static Style* GetStyle(napi_env env, napi_value value) noexcept {
  Style* style{};

  if (Habitat::InstanceOf(env, value, Habitat::Class::CStyle)) {
    style = napix::unwrap_as<Style>(env, value);
  }

  NAPIX_EXPECT_NOT_NULL(env, style, "Expected a Style instance", {})

  return style;
}

napi_value ParseColor(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};

  return BoxColor(env, UnboxColor(env, ci[0]));
}

napi_value ParseValue(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto styleValue{ UnboxStyleValue(env, ci[0])};

  return BoxStyleValue(env, styleValue.value_or(StyleValue::OfUndefined()));
}

napi_value LoadSDLPlugin(napi_env env, napi_callback_info) noexcept {
  if (kEnablePluginPlatformSdl) {
    return LoadSDLPlatformPlugin(env);
  }

  napix::throw_error(env, "SDL plugin is not available.");
}

napi_value LoadRefPlugin(napi_env env, napi_callback_info) noexcept {
  if (Habitat::HasClass(env, Habitat::Class::CGraphicsContext)) {
    napix::throw_error(env, "");
    return {};
  }

  return Habitat::SetClass(env, Habitat::Class::CGraphicsContext, CRefGraphicsContext::CreateClass(env));
}

Napi::Value LoadSDLAudioPlugin(const Napi::CallbackInfo& info) {
  if (kEnablePluginAudioSdlAudio) {
    SDL2::Open();
    return lse::AudioPluginInit<lse::SDLAudioPluginImpl>(info.Env());
  }

  throw Napi::Error::New(info.Env(), "SDL Audio plugin is not available.");
}

Napi::Value LoadSDLMixerPlugin(const Napi::CallbackInfo& info) {
  if (kEnablePluginAudioSdlMixer) {
    SDL2::Open();
    SDL2::mixer::Open();
    return lse::AudioPluginInit<lse::SDLMixerAudioPluginImpl>(info.Env());
  }

  throw Napi::Error::New(info.Env(), "SDL Audio plugin is not available.");
}

napi_value LockStyle(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto style{GetStyle(env, ci[0])};

  if (style) {
    style->Lock();
  }

  return {};
}

napi_value SetStyleParent(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<2>(env, info)};
  Style* style{};
  Style* parent{};

  if (Habitat::InstanceOf(env, ci[0], Habitat::Class::CStyle)) {
    style = napix::unwrap_as<Style>(env, ci[0]);
  }

  NAPIX_EXPECT_TRUE(env, style && !style->IsLocked(), "arg is not a Style instance.", {})

  if (napix::is_nullish(env, ci[1])) {
    parent = nullptr;
  } else if (Habitat::InstanceOf(env, ci[1], Habitat::Class::CStyle)) {
    parent = napix::unwrap_as<Style>(env, ci[1]);
    NAPIX_EXPECT_TRUE(env, parent && parent->IsLocked(), "arg is not a StyleClass instance.", {})
  } else {
    napix::throw_error(env, "Expected StyleClass, null or undefined");
  }

  style->SetParent(parent);

  return {};
}

napi_value ResetStyle(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto style{GetStyle(env, ci[0])};

  if (style) {
    style->Reset();
  }

  return {};
}

napi_value InstallStyleValue(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};

  return Habitat::SetClass(env, Habitat::Class::StyleValue, ci[0]);
}

napi_value InstallStyleTransformSpec(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};

  return Habitat::SetClass(env, Habitat::Class::StyleTransformSpec, ci[0]);
}

napi_value GetSceneNodeInstanceCount(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, SceneNode::GetInstanceCount());
}

} // namespace bindings
} // namespace lse
