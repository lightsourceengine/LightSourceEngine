/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "Resource.h"
#include <napi.h>
#include <memory>

struct stbtt_fontinfo;

namespace ls {

class ResourceManager;
class AsyncTaskQueue;
class Task;

class FontResource : public Resource {
 public:
    explicit FontResource(const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    virtual ~FontResource() = default;

    static std::string MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    const std::string& GetFontFamily() const { return this->family; }
    StyleFontStyle GetFontStyle() const { return this->fontStyle; }
    StyleFontWeight GetFontWeight() const { return this->fontWeight; }

    std::shared_ptr<stbtt_fontinfo> GetFontInfo() const { return this->fontInfo; }

    Napi::Value ToObject(Napi::Env env) const;

 private:
    std::string uri;
    int32_t index;
    std::string family;
    StyleFontStyle fontStyle{};
    StyleFontWeight fontWeight{};
    mutable std::shared_ptr<stbtt_fontinfo> fontInfo;
    std::shared_ptr<Task> task;

 private:
    void Load(AsyncTaskQueue* taskQueue, const std::vector<std::string>& path);

    friend ResourceManager;
};

} // namespace ls
