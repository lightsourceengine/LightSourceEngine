/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include <cstring>
#include <stb_truetype.h>
#include <fmt/format.h>

using Napi::AsyncTask;

namespace ls {

std::shared_ptr<stbtt_fontinfo> LoadFont(
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
    auto path{ resourcePath };

    try {
        this->task = std::make_unique<AsyncTask<stbtt_fontinfo>>(
            this->env,
            this->id,
            [path, uri, index]() -> std::shared_ptr<stbtt_fontinfo> {
                return LoadFont(path, uri, index);
            },
            [this](Napi::Env env, std::shared_ptr<stbtt_fontinfo> result, napi_status status,
                    const std::string& message) {
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

std::shared_ptr<stbtt_fontinfo> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    std::shared_ptr<stbtt_fontinfo> result(new stbtt_fontinfo(),
        [](stbtt_fontinfo* p){ if (p->data) { delete [] p->data; } delete p; });
    std::string resolvedFilename;

    std::memset(result.get(), 0, sizeof(stbtt_fontinfo));

    if (IsResourceUri(filename)) {
        resolvedFilename = FindFile(GetResourceUriPath(filename), {}, path);
    } else {
        resolvedFilename = filename;
    }

    FileHandle file(fopen(resolvedFilename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to open ttf file: {}", filename));
    }

    fseek(file.get(), 0, SEEK_END);
    auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    std::unique_ptr<uint8_t[]> ttf(new uint8_t[size]);

    if (fread(ttf.get(), 1, size, file.get()) != size) {
        throw std::runtime_error(fmt::format("Failed to read file: {}", filename));
    }

    auto offset{ stbtt_GetFontOffsetForIndex(ttf.get(), index) };

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(result.get(), ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    ttf.release();

    return result;
}

} // namespace ls
