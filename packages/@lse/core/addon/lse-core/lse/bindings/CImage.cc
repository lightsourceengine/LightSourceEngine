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
#include <lse/Image.h>

namespace lse {
namespace bindings {

bool CImage::UnboxImageRequest(napi_env env, napi_value value, ImageRequest& imageRequest) noexcept {
  if (!value) {
    return false;
  }

  imageRequest.uri = napix::object_get(env, value, "uri");
  imageRequest.width = napix::object_get_or(env, value, "width", 0);
  imageRequest.height = napix::object_get_or(env, value, "height", 0);

  // TODO: validate!

  return true;
}

} // namespace bindings
} // namespace lse
