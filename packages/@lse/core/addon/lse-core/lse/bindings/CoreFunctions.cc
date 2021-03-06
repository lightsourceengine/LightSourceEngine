/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/bindings/CoreFunctions.h>

#include <napix.h>
#include <lse/Habitat.h>
#include <lse/bindings/CStyleUtil.h>
#include <lse/Config.h>
#include <lse/bindings/SDLPlatformPluginExports.h>
#include <lse/bindings/SDLAudioExports.h>
#include <lse/bindings/SDLMixerExports.h>
#include <lse/bindings/CoreClasses.h>
#include <lse/SceneNode.h>

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

napi_value LoadSDLPlugin(napi_env env, napi_callback_info info) noexcept {
  if (kEnablePluginPlatformSdl) {
    auto ci{napix::get_callback_info<1>(env, info)};

    return LoadSDLPlatformPlugin(env, ci[0]);
  }

  napix::throw_error(env, "SDL plugin is not available.");
  return {};
}

napi_value LoadRefPlugin(napi_env env, napi_callback_info) noexcept {
  if (Habitat::HasClass(env, Habitat::Class::CGraphicsContext)) {
    napix::throw_error(env, "graphics plugin already installed?");
    return {};
  }

  return Habitat::SetClass(env, Habitat::Class::CGraphicsContext, CRefGraphicsContext::CreateClass(env));
}

napi_value LoadSDLAudioPlugin(napi_env env, napi_callback_info info) noexcept {
  if (kEnablePluginAudioSdlAudio) {
    auto ci{napix::get_callback_info<1>(env, info)};

    return CreateSDLAudioPlugin(env, ci[0]);
  }

  napix::throw_error(env, "SDL Audio plugin is not available.");
  return {};
}

napi_value LoadSDLMixerPlugin(napi_env env, napi_callback_info info) noexcept {
  if (kEnablePluginAudioSdlMixer) {
    auto ci{napix::get_callback_info<1>(env, info)};

    return CreateSDLMixerAudioPlugin(env, ci[0]);
  }

  napix::throw_error(env, "SDL Mixer plugin is not available.");
  return {};
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

static napi_value SetClass(napi_env env, napi_callback_info info, Habitat::Class::Enum classId) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};

  return Habitat::SetClass(env, classId, ci[0]);
}

napi_value InstallStyleValue(napi_env env, napi_callback_info info) noexcept {
  return SetClass(env, info, Habitat::Class::StyleValue);
}

napi_value InstallStyleTransformSpec(napi_env env, napi_callback_info info) noexcept {
  return SetClass(env, info, Habitat::Class::StyleTransformSpec);
}

napi_value InstallImage(napi_env env, napi_callback_info info) noexcept {
  return SetClass(env, info, Habitat::Class::CImage);
}

napi_value GetSceneNodeInstanceCount(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, SceneNode::GetInstanceCount());
}

} // namespace bindings
} // namespace lse
