/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/RootSceneNode.h>
#include <lse/bindings/CSceneNodeConstructor.h>

using napix::js_class::define;

namespace lse {
namespace bindings {

napi_value CRootSceneNode::CreateClass(napi_env env) noexcept {
  auto props{ CSceneNode::GetClassProperties(env) };

  return define(env, NAME, &CSceneNodeConstructor<RootSceneNode>, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
