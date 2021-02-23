/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
