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

#include "Habitat.h"

#include <array>
#include <string>
#include <memory>
#include <cassert>

namespace lse {

constexpr size_t MAX_APP_DATA_ENTRIES = 3;

struct AppDataEntry {
  std::string key{};
  void* data{};
  Habitat::AppDataFinalizer finalizer{};
};

struct InstanceData {
  // app data is searched by key, but the list is so small a map is too much overhead
  std::array<AppDataEntry, MAX_APP_DATA_ENTRIES> appData{};
  std::array<napi_ref, Habitat::Class::Count> classRefs{};
};

static AppDataEntry* FindByKey(InstanceData* instanceData, const char* key) noexcept {
  for (auto& entry : instanceData->appData) {
    if (key != nullptr && entry.key == key) {
      return &entry;
    }
  }

  return {};
}

static AppDataEntry* FindEmpty(InstanceData* instanceData) noexcept {
  for (auto& entry : instanceData->appData) {
    if (entry.key.empty()) {
      return &entry;
    }
  }

  return {};
}

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
          if (entry.finalizer) {
            entry.finalizer(entry.data);
          }
        }

        for (napi_ref ref : instanceData->classRefs) {
          napi_delete_reference(env, ref);
        }

        delete instanceData;
      },
      nullptr);

  if (status == napi_ok) {
    // NOLINTNEXTLINE ptr owned by node now
    instanceData.release();
    return true;
  }

  napi_throw_error(env, "", "Failed to set environment instance data.");

  return false;
}

napi_value Habitat::SetClass(napi_env env, Class::Enum classId, napi_value constructor) noexcept {
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

bool Habitat::HasClass(napi_env env, Class::Enum classId) noexcept {
  auto instanceData{ GetInstanceData(env) };

  return (instanceData && instanceData->classRefs[classId]);
}

bool Habitat::InstanceOf(napi_env env, napi_value obj, Class::Enum classId) noexcept {
  auto constructor{ Habitat::GetClass(env, classId) };
  bool result{};

  return constructor && napi_instanceof(env, obj, constructor, &result) == napi_ok && result;
}

bool Habitat::SetAppData(napi_env env, const char* key, void* data, Habitat::AppDataFinalizer finalizer) noexcept {
  if (!key) {
    return false;
  }

  auto instanceData{ GetInstanceData(env) };

  if (instanceData) {
    auto entry{FindByKey(instanceData, key)};

    if (entry) {
      if (entry->data && entry->finalizer) {
        entry->finalizer(entry->data);
      }

      entry->data = data;
      entry->finalizer = finalizer;

      return true;
    }

    entry = FindEmpty(instanceData);

    assert(entry);

    if (entry) {
      *entry = { key, data, finalizer };

      return true;
    }
  }

  return false;
}

void* Habitat::GetAppData(napi_env env, const char* key) noexcept {
  auto instanceData{ GetInstanceData(env) };

  if (instanceData) {
    auto entry{FindByKey(instanceData, key)};

    if (entry) {
      return entry->data;
    }
  }

  return {};
}

} // namespace lse
