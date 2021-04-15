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

#include "CoreEnums.h"

#include <lse/StyleEnums.h>
#include <napix.h>

using napix::object_new;
using napix::descriptor::instance_value;

namespace lse {
namespace bindings {

napi_value NewStyleUnitEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "UNDEFINED", StyleNumberUnitUndefined, napi_enumerable),
      instance_value(env, "POINT", StyleNumberUnitPoint, napi_enumerable),
      instance_value(env, "PERCENT", StyleNumberUnitPercent, napi_enumerable),
      instance_value(env, "VIEWPORT_WIDTH", StyleNumberUnitViewportWidth, napi_enumerable),
      instance_value(env, "VIEWPORT_HEIGHT", StyleNumberUnitViewportHeight, napi_enumerable),
      instance_value(env, "VIEWPORT_MIN", StyleNumberUnitViewportMin, napi_enumerable),
      instance_value(env, "VIEWPORT_MAX", StyleNumberUnitViewportMax, napi_enumerable),
      instance_value(env, "AUTO", StyleNumberUnitAuto, napi_enumerable),
      instance_value(env, "ANCHOR", StyleNumberUnitAnchor, napi_enumerable),
      instance_value(env, "REM", StyleNumberUnitRootEm, napi_enumerable),
      instance_value(env, "RADIAN", StyleNumberUnitRadian, napi_enumerable),
      instance_value(env, "DEGREE", StyleNumberUnitDegree, napi_enumerable),
      instance_value(env, "GRADIAN", StyleNumberUnitGradian, napi_enumerable),
      instance_value(env, "TURN", StyleNumberUnitTurn, napi_enumerable),
  });
}

napi_value NewStyleTransformEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "IDENTITY", StyleTransformIdentity, napi_enumerable),
      instance_value(env, "TRANSLATE", StyleTransformTranslate, napi_enumerable),
      instance_value(env, "ROTATE", StyleTransformRotate, napi_enumerable),
      instance_value(env, "SCALE", StyleTransformScale, napi_enumerable),
  });
}

napi_value NewStyleAnchorEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "LEFT", StyleAnchorLeft, napi_enumerable),
      instance_value(env, "RIGHT", StyleAnchorRight, napi_enumerable),
      instance_value(env, "CENTER", StyleAnchorCenter, napi_enumerable),
      instance_value(env, "TOP", StyleAnchorTop, napi_enumerable),
      instance_value(env, "BOTTOM", StyleAnchorBottom, napi_enumerable),
  });
}

napi_value NewStyleFilterEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "TINT", StyleFilterTint, napi_enumerable),
      instance_value(env, "FLIP_H", StyleFilterFlipH, napi_enumerable),
      instance_value(env, "FLIP_V", StyleFilterFlipV, napi_enumerable),
  });
}

} // namespace bindings
} // namespace lse
