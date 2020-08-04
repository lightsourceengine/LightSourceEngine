/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <blend2d.h>
#include <ls/AsyncWork.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace ls {

class Res {
 public:
    using Owner = void*;
    using Listener = std::function<void(Owner, Res*)>;

    enum State {
        Init,
        Ready,
        Error,
        Loading,
    };

 public:
    Res(const std::string& id) : id(id) {}
    virtual ~Res() = default;

    void AddListener(Owner owner, Listener&& listener);
    void RemoveListener(Owner owner);
    State GetState() const noexcept { return this->state; }
    virtual void Load(Napi::Env env) = 0;

    Napi::String GetErrorMessage(const Napi::Env& env) const;
    virtual Napi::Value GetSummary(const Napi::Env& env) const = 0;

 protected:
    void NotifyListeners();

 protected:
    struct ListenerEntry {
        Owner owner{};
        Listener listener;
    };

    std::string id;
    std::vector<ListenerEntry> listeners;
    State state{ Init };
    std::string errorMessage;

    friend class Resources;
};

class ImageData : public Res {
 public:
    explicit ImageData(const std::string& id) : Res(id) {}
    ~ImageData() override = default;

    void Load(Napi::Env env) override;
    Napi::Value GetSummary(const Napi::Env& env) const override;

 private:
    AsyncWork<BLImage> work;
    BLImage resource;
};

class FontFace : public Res {
 public:
    explicit FontFace(const std::string& id) : Res(id) {}
    ~FontFace() override = default;

    void Load(Napi::Env env) override;
    Napi::Value GetSummary(const Napi::Env& env) const override;

 private:
    AsyncWork<BLFontFace> work;
    BLFontFace resource;
};

class Resources {
 public:
    bool HasImageData(const std::string& path) const;
    ImageData* AcquireImageData(const std::string& path);
    void ReleaseImageData(const std::string& path, bool immediateDelete = false);

    bool HasFontFace(const std::string& path) const;
    FontFace* AcquireFontFace(const std::string& path);
    void ReleaseFontFace(const std::string& path, bool immediateDelete = false);

    void ReleaseResource(Res* resource, bool immediateDelete = false);

    void Compact();

 private:
    template <typename T>
    struct ResourceRef {
        ResourceRef() noexcept : refs(0), resource(nullptr) {}
        explicit ResourceRef(std::unique_ptr<T>&& p) noexcept : refs(1), resource(std::move(p)) {}
        ResourceRef(ResourceRef<T>&& other) noexcept : refs(other.refs), resource(std::move(other.resource)) {}

        T* ToPointer() const noexcept { return this->resource.get(); }

        int32_t refs{};
        std::unique_ptr<T> resource;
    };

    using ImageDataRef = ResourceRef<ImageData>;
    using FontFaceRef = ResourceRef<FontFace>;

    using ImageDataMap = std::unordered_map<std::string, ImageDataRef>;
    using FontFaceMap = std::unordered_map<std::string, FontFaceRef>;

    ImageDataMap images;
    FontFaceMap fonts;
};

} // namespace ls
