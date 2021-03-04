/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
      instance_value(env, "Undefined", StyleNumberUnitUndefined, napi_enumerable),
      instance_value(env, "Point", StyleNumberUnitPoint, napi_enumerable),
      instance_value(env, "Percent", StyleNumberUnitPercent, napi_enumerable),
      instance_value(env, "ViewportWidth", StyleNumberUnitViewportWidth, napi_enumerable),
      instance_value(env, "ViewportHeight", StyleNumberUnitViewportHeight, napi_enumerable),
      instance_value(env, "ViewportMin", StyleNumberUnitViewportMin, napi_enumerable),
      instance_value(env, "ViewportMax", StyleNumberUnitViewportMax, napi_enumerable),
      instance_value(env, "Auto", StyleNumberUnitAuto, napi_enumerable),
      instance_value(env, "Anchor", StyleNumberUnitAnchor, napi_enumerable),
      instance_value(env, "RootEm", StyleNumberUnitRootEm, napi_enumerable),
      instance_value(env, "Radian", StyleNumberUnitRadian, napi_enumerable),
      instance_value(env, "Degree", StyleNumberUnitDegree, napi_enumerable),
      instance_value(env, "Gradian", StyleNumberUnitGradian, napi_enumerable),
      instance_value(env, "Turn", StyleNumberUnitTurn, napi_enumerable),
  });
}

napi_value NewStyleTransformEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "Identity", StyleTransformIdentity, napi_enumerable),
      instance_value(env, "Translate", StyleTransformTranslate, napi_enumerable),
      instance_value(env, "Rotate", StyleTransformRotate, napi_enumerable),
      instance_value(env, "Scale", StyleTransformScale, napi_enumerable),
  });
}

napi_value NewStyleAnchorEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "Left", StyleAnchorLeft, napi_enumerable),
      instance_value(env, "Right", StyleAnchorRight, napi_enumerable),
      instance_value(env, "Center", StyleAnchorCenter, napi_enumerable),
      instance_value(env, "Top", StyleAnchorTop, napi_enumerable),
      instance_value(env, "Bottom", StyleAnchorBottom, napi_enumerable),
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
