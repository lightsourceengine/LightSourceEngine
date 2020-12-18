/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {

class SceneNode;
class Resource;

/**
 * Composite class that adds onLoad and onError resource properties to SceneNodes (img and link).
 */
class ResourceProgress {
 public:
    virtual ~ResourceProgress() noexcept = default;

    void Dispatch(SceneNode* owner, Resource* resource) const;
    void Reset();

    void SetOnLoad(const Napi::Env& env, const Napi::Value& value);
    Napi::Value GetOnLoad(const Napi::Env& env) const;
    void SetOnError(const Napi::Env& env, const Napi::Value& value);
    Napi::Value GetOnError(const Napi::Env& env) const;

 private:
    Napi::FunctionReference onLoad{};
    Napi::FunctionReference onError{};
};

} // namespace ls

