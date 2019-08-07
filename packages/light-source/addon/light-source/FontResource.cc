/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include "AsyncTaskQueue.h"
#include <stb_truetype.h>
#include <fmt/format.h>
#include <cstring>

using Napi::EscapableHandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

std::shared_ptr<TaskResult> LoadFont(
    const std::vector<std::string>& path, const std::string& filename, const int32_t index);

struct FontTaskResult : public TaskResult {
    FontTaskResult()
    : fontInfo(new stbtt_fontinfo(), &FontTaskResult::FontInfoDeleter) {
        this->fontInfo->data = nullptr;
    }

    static void FontInfoDeleter(stbtt_fontinfo* p) {
        if (p->data) {
            delete [] p->data;
        }

        delete p;
    }

    std::shared_ptr<stbtt_fontinfo> fontInfo;
};

FontResource::FontResource(const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight)
    : Resource(id), uri(uri), index(index), family(family), fontStyle(fontStyle), fontWeight(fontWeight) {
}

std::string FontResource::MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight) {
    return fmt::format("{}:{}:{}", family, fontStyle, fontWeight);
}

void FontResource::Load(AsyncTaskQueue* taskQueue, const std::vector<std::string>& resourcePath) {
    auto initialState{ ResourceStateLoading };
    auto uri{ this->uri };
    auto index{ this-> index };
    auto path{ resourcePath };

    assert(!this->task);

    try {
        this->task = std::make_shared<Task>(
            [path, uri, index]() -> std::shared_ptr<TaskResult> {
                return LoadFont(path, uri, index);
            },
            [this](std::shared_ptr<TaskResult> taskResult) {
                assert(taskResult != nullptr);

                // TODO: assert(this->resourceState != ResourceStateLoading)
                // TODO: assert(this->GetRefCount() > 0)

                auto fontTaskResult{ std::static_pointer_cast<FontTaskResult>(taskResult) };

                this->fontInfo = fontTaskResult->fontInfo;
                this->task = nullptr;

                this->SetStateAndNotifyListeners(ResourceStateReady);
            },
            [this](const std::string& errorMessage) {
                fmt::println("FontResource.Load(): Error: {}", errorMessage);
                this->SetStateAndNotifyListeners(ResourceStateError);
            });
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    try {
        if (this->task) {
            taskQueue->Submit(this->task);
        }
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

std::shared_ptr<TaskResult> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    auto result{ std::make_shared<FontTaskResult>() };
    std::string resolvedFilename;

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

    if (stbtt_InitFont(result->fontInfo.get(), ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    ttf.release();

    return result;
}

Value FontResource::ToObject(Napi::Env env) {
    EscapableHandleScope scope(env);
    auto font{ Object::New(env) };

    font["family"] = String::New(env, this->family);
    font["style"] = String::New(env, StyleFontStyleToString(this->fontStyle));
    font["weight"] = String::New(env, StyleFontWeightToString(this->fontWeight));
    font["index"] = Number::New(env, this->index);
    font["resourceId"] = String::New(env, this->GetId());
    font["refs"] = Number::New(env, this->GetRefCount());
    font["state"] = String::New(env, ResourceStateToString(this->resourceState));

    return scope.Escape(font);
}

} // namespace ls
