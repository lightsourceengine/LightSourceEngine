/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "Resource.h"
#include <memory>
#include <unordered_map>

struct stbtt_fontinfo;

namespace ls {

class ResourceManager;
class AsyncTaskQueue;
class Task;
class Font;

struct FontId {
    std::string family;
    StyleFontStyle style{};
    StyleFontWeight weight{};

    bool operator==(const FontId &other) const {
        return this->family == other.family && this->style == other.style && this->weight == other.weight;
    }

    struct Hash {
        std::size_t operator() (const FontId& id) const {
            return std::hash<std::string>()(id.family)
                ^ static_cast<std::size_t>(id.style)
                ^ static_cast<std::size_t>(id.weight);
        }
    };
};

class FontResource : public BaseResource<FontId> {
 public:
    FontResource(const FontId& fontId, const std::string& uri, const int32_t index);
    virtual ~FontResource() = default;

    const std::string& GetFontFamily() const { return this->id.family; }
    StyleFontStyle GetFontStyle() const { return this->id.style; }
    StyleFontWeight GetFontWeight() const { return this->id.weight; }
    int32_t GetIndex() const { return this->index; }
    const std::string GetUri() const { return this->uri; }
    std::shared_ptr<Font> GetFont(int32_t fontSize) const;

    void Load(Stage* stage);
    void Attach(Stage* stage, Scene* scene) override;

 private:
    std::string uri;
    int32_t index{0};
    mutable std::shared_ptr<stbtt_fontinfo> fontInfo;
    mutable std::unordered_map<int32_t, std::shared_ptr<Font>> fontsBySize;
    std::shared_ptr<Task> task;
};

} // namespace ls
