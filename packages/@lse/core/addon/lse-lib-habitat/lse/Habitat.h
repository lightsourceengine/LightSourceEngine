/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <node_api.h>

namespace lse {

/**
 * Node environment instance data for the Light Source Engine addon.
 *
 * The instance contains class objects that are passed between the native
 * layer and javascript. The native code needs to access these classes to
 * create new objects, perform instanceOf checks, etc.
 */
class Habitat {
 public:
  struct Class {
    enum Enum {
      CStage,
      CFontManager,
      CScene,
      Count
    };
  };

  using AppDataFinalizer = void(*)(void*);

 public:
  /**
   * Initialize the Habitat in the environment.
   *
   * After Init, the Habitat is ready, but no classes will be installed. The caller
   * of Init will install classes as necessary.
   *
   * If the call fails, a node environment exception is thrown.
   *
   * @param env node environment
   * @return true if initialize; false otherwise
   */
  static bool Init(napi_env env) noexcept;

  /**
   * Add a class constructor for the given class id.
   *
   * If constructor is invalid or class install fails, a javascript exception is thrown.
   *
   * @param env node environment
   * @param classId class to install
   * @param constructor class constructor
   */
  static napi_value LoadClass(napi_env env, Class::Enum classId, napi_value constructor) noexcept;

  /**
   * Get a class for the given class id.
   *
   * No javascript exception is thrown.
   *
   * @param env node environment
   * @param classId class to get
   * @return class constructor or nullptr on failure
   */
  static napi_value GetClass(napi_env env, Class::Enum classId) noexcept;

  /**
   * Checks if an object is an instance of a class.
   *
   * No javascript exception is thrown.
   *
   * @param env node environment
   * @param obj object to test
   * @param classId class to test against
   * @return true if obj is instance of class; false otherwise
   */
  static bool InstanceOf(napi_env env, napi_value obj, Class::Enum classId) noexcept;

  /**
   *
   * @param env
   * @param key
   * @param data
   * @param finalizer
   */
  static bool SetAppData(napi_env env, const char* key, void* data, AppDataFinalizer finalizer) noexcept;

  /**
   *
   * @param env
   * @param key
   * @return
   */
  static void* GetAppData(napi_env env, const char* key) noexcept;

  template<typename T>
  static T* GetAppDataAs(napi_env env, const char* key) noexcept {
    return static_cast<T*>(GetAppData(env, key));
  }
};

} // namespace lse
