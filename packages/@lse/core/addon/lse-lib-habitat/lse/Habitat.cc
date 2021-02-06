/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "Habitat.h"

#include <array>
#include <memory>
#include <phmap.h>

namespace lse {

struct AppDataEntry {
  void* data{};
  Habitat::AppDataFinalizer finalizer{};
};

struct InstanceData {
  phmap::flat_hash_map<std::string, AppDataEntry> appData{};
  std::array<napi_ref, Habitat::Class::Count> classRefs{};
};

static InstanceData* GetInstanceData(napi_env env) noexcept {
  void* data{};

  if (napi_get_instance_data(env, &data) == napi_ok) {
    return static_cast<InstanceData*>(data);
  }

  return {};
}

static bool HasPendingException(napi_env env) noexcept {
  bool result{};
  return (napi_is_exception_pending(env, &result) != napi_ok || result);
}

bool Habitat::Init(napi_env env) noexcept {
  if (HasPendingException(env)) {
    return false;
  }

  if (GetInstanceData(env)) {
    napi_throw_error(env, "", "Environment instance data has already been initialized.");
    return false;
  }

  auto instanceData{ std::make_unique<InstanceData>() };
  auto status = napi_set_instance_data(
      env,
      instanceData.get(),
      [](napi_env env, void* data, void*) {
        auto instanceData{ static_cast<InstanceData*>(data) };

        for (auto& entry : instanceData->appData) {
          if (entry.second.finalizer) {
            entry.second.finalizer(entry.second.data);
          }
        }

        for (napi_ref ref : instanceData->classRefs) {
          napi_delete_reference(env, ref);
        }

        delete instanceData;
      },
      nullptr);

  if (status == napi_ok) {
    instanceData.release();
    return true;
  }

  napi_throw_error(env, "", "Failed to set environment instance data.");

  return false;
}

napi_value Habitat::LoadClass(napi_env env, Class::Enum classId, napi_value constructor) noexcept {
  if (HasPendingException(env)) {
    return {};
  }

  napi_valuetype type{};

  if (constructor) {
    napi_typeof(env, constructor, &type);
  }

  if (type != napi_function) {
    napi_throw_error(env, "", "Invalid class constructor.");
    return {};
  }

  auto instanceData{ GetInstanceData(env) };

  if (!instanceData) {
    napi_throw_error(env, "", "No environment instance data.");
    return {};
  }

  if (instanceData->classRefs[classId]) {
    napi_throw_error(env, "", "Class already added to environment instance.");
    return {};
  }

  napi_ref ref{};
  auto status = napi_create_reference(env, constructor, 1, &ref);

  if (status != napi_ok) {
    napi_throw_error(env, "", "Failed to create ref for constructor.");
    return {};
  }

  instanceData->classRefs[classId] = ref;

  return constructor;
}

napi_value Habitat::GetClass(napi_env env, Class::Enum classId) noexcept {
  auto instanceData{ GetInstanceData(env) };

  if (instanceData && instanceData->classRefs[classId]) {
    napi_value constructor{};

    if (napi_get_reference_value(env, instanceData->classRefs[classId], &constructor) == napi_ok) {
      return constructor;
    }
  }

  return {};
}

bool Habitat::InstanceOf(napi_env env, napi_value obj, Class::Enum classId) noexcept {
  auto constructor{ Habitat::GetClass(env, classId) };
  bool result{};

  return constructor && napi_instanceof(env, obj, constructor, &result) == napi_ok && result;
}

bool Habitat::SetAppData(napi_env env, const char* key, void* data, Habitat::AppDataFinalizer finalizer) noexcept {
  auto instanceData{ GetInstanceData(env) };

  if (instanceData) {
    instanceData->appData.insert_or_assign(key, { data, finalizer });
    return true;
  }

  return false;
}

void* Habitat::GetAppData(napi_env env, const char* key) noexcept {
  auto instanceData{ GetInstanceData(env) };

  if (instanceData && instanceData->appData.contains(key)) {
    return instanceData->appData[key].data;
  }

  return nullptr;
}

} // namespace lse
