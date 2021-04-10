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
