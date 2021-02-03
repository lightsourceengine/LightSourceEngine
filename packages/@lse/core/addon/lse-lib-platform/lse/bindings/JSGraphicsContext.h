/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/GraphicsContext.h>

namespace lse {
namespace bindings {

/**
 * Bindings for GraphicsContext.
 *
 * The full GraphicsContext interface is not exported to javascript. The behavior dealing with
 * window properties are exported and the rendering related behavior stays native for performance
 * reasons.
 */
class JSGraphicsContext {
 public:
  /**
   * Create a new GraphicsContext object.
   *
   * @tparam T Native implementation of GraphicsContext.
   */
  template<typename T>
  static Napi::Value New(const Napi::Env& env, const Napi::Value& options) {
    GraphicsContextRef ptr = std::make_shared<T>();
    return JSGraphicsContext::New(env, options, ptr);
  }

 private:
  static Napi::Value New(const Napi::Env& env, const Napi::Value& options, GraphicsContextRef& context);
};

} // namespace bindings
} // namespace lse
