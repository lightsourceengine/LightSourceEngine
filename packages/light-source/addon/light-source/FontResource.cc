/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include <fmt/format.h>

using Napi::AsyncTask;

namespace ls {

std::shared_ptr<FontInfo> LoadFont(
    const std::vector<std::string>& path, const std::string& filename, const int32_t index);

FontResource::FontResource(Napi::Env env, const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight)
    : Resource(env, id), uri(uri), index(index), family(family), fontStyle(fontStyle), fontWeight(fontWeight) {
}

std::string FontResource::MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight) {
    return fmt::format("{}:{}:{}", family, fontStyle, fontWeight);
}

void FontResource::Load(const std::vector<std::string>& resourcePath) {
    auto initialState{ ResourceStateLoading };
    auto uri{ this->uri };
    auto index{ this-> index };

    try {
        this->task = std::make_unique<AsyncTask<FontInfo>>(
            this->env,
            this->id,
            [resourcePath, uri, index](Napi::Env env) -> std::shared_ptr<FontInfo> {
                return LoadFont(resourcePath, uri, index);
            },
            [this](Napi::Env env, std::shared_ptr<FontInfo> result, napi_status status, const std::string& message) {
                // TODO: assert(this->resourceState != ResourceStateLoading)
                // TODO: assert(this->GetRefCount() > 0)

                this->fontInfo = result;
                this->task.reset();

                this->SetStateAndNotifyListeners(status != napi_ok ? ResourceStateError : ResourceStateReady);
            });
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

std::shared_ptr<FontInfo> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    auto result{ std::make_shared<FontInfo>() };
    std::string resolvedFilename;

    if (IsResourceUri(filename)) {
        resolvedFilename = FindFile(GetResourceUriPath(filename), {}, path);
    } else {
        resolvedFilename = filename;
    }

    FileHandle file(fopen(filename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to open ttf file: {}", filename));
    }

    fseek(file.get(), 0, SEEK_END);
    auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    result->ttf.reset(new uint8_t[size]);

    if (fread(result->ttf.get(), 1, size, file.get()) != size) {
        throw std::runtime_error(fmt::format("Failed to read file: {}", filename));
    }

    auto offset{ stbtt_GetFontOffsetForIndex(result->ttf.get(), index) };

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(&(result->stbFontInfo), result->ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    return result;
}

} // namespace ls
