/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include <fmt/format.h>

namespace ls {

std::vector<uint8_t> LoadFont(const std::vector<std::string>& path, const std::string& filename, const int32_t index,
    stbtt_fontinfo* info);

FontResource::FontResource(Napi::Env env, const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight)
    : Resource(env, id), uri(uri), index(index), family(family), fontStyle(fontStyle), fontWeight(fontWeight) {
}

std::string FontResource::MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight) {
    return fmt::format("{}:{}:{}", family, fontStyle, fontWeight);
}

void FontResource::Load(const std::vector<std::string>& path) {
    auto initialState{ ResourceStateLoading };

    // TODO: what if ref == 0?
    this->AddRef();

    try {
        this->work = std::make_unique<AsyncWork>(
            this->env,
            this->id,
            [this, path](Napi::Env env) {
                // TODO: do not modify resource here... execute should return a value and complete can modify
                this->fontInfoData = LoadFont(path, this->uri, this->index, &this->fontInfo);
            },
            [this](Napi::Env env, napi_status status, const std::string& message) {
                if (this->resourceState != ResourceStateLoading) {
                    fmt::println("Warning: AsyncWork returned to a resource({}) in an unexpected state.",
                        this->GetId());
                    return;
                }

                this->RemoveRef();
                this->work.reset();

                if (this->GetRefCount() > 0) {
                    ResourceState nextState;

                    if (status != napi_ok) {
                        nextState = ResourceStateError;
                    } else {
                        nextState = ResourceStateReady;
                    }

                    this->SetStateAndNotifyListeners(nextState);
                }
            });
    } catch (std::exception& e) {
        this->RemoveRef();
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

std::vector<uint8_t> LoadFont(const std::vector<std::string>& path, const std::string& filename, const int32_t index,
        stbtt_fontinfo* info) {
    static const std::vector<std::string> emptyVector = {};
    std::string resolvedFilename;

    if (IsResourceUri(filename)) {
        resolvedFilename = FindFile(GetResourceUriPath(filename), emptyVector, path);
    } else {
        resolvedFilename = filename;
    }

    auto buffer{ ReadBytes(resolvedFilename) };
    auto offset = stbtt_GetFontOffsetForIndex(&buffer[0], index);

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(info, &buffer[0], offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    return buffer;
}

} // namespace ls
