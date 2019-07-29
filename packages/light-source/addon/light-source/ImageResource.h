/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Resource.h"

namespace ls {

class ResourceManager;

class ImageResource : public Resource {
 public:
    explicit ImageResource(const std::string& id);
    virtual ~ImageResource() = default;

 protected:
    void Load();
    void Sync();

    friend ResourceManager;
};

} // namespace ls
