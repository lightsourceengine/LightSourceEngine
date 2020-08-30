/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <blend2d.h>
#include <ls/AsyncWork.h>
#include <ls/StyleEnums.h>
#include <ls/Texture.h>
#include <ls/ImageBytes.h>

#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <std17/filesystem>

namespace ls {

class Renderer;

class Resource {
 public:
    using Owner = void*;
    using Listener = std::function<void(Owner, Resource*)>;

    enum State {
        Init,
        Ready,
        Error,
        Loading,
    };

 public:
    Resource(const std::string& id);
    virtual ~Resource() = default;

    void AddListener(Owner owner, Listener&& listener);
    void RemoveListener(Owner owner);
    State GetState() const noexcept { return this->state; }
    const std::string& GetId() const noexcept { return this->id; }
    virtual void Load(Napi::Env env) = 0;

    Napi::String GetErrorMessage(const Napi::Env& env) const;
    virtual Napi::Value Summarize(const Napi::Env& env) const = 0;

 protected:
    void NotifyListeners();

 protected:
    struct ListenerEntry {
        Owner owner{};
        Listener listener;
    };

    std::string id{};
    std17::filesystem::path path{};
    std::vector<ListenerEntry> listeners{};
    State state{ Init };
    std::string errorMessage{};

    friend class Resources;
};

class Image final : public Resource {
 public:
    Image(const std::string& id) : Resource(id) {}
    ~Image() override = default;

    void Load(Napi::Env env) override;
    Napi::Value Summarize(const Napi::Env& env) const override;

    bool LoadTexture(Renderer* renderer);
    bool HasTexture() const noexcept;
    Texture GetTexture() const noexcept;

    bool HasDimensions() const noexcept;
    int32_t Width() const noexcept;
    int32_t Height() const noexcept;
    float WidthF() const noexcept;
    float HeightF() const noexcept;
    float AspectRatio() const noexcept;

    // Test-only method.
    static Image Mock(const std::string& id, int32_t width, int32_t height);

 private:
    AsyncWork<ImageBytes> work;
    ImageBytes resource{};
    Texture texture{};
    int32_t width{};
    int32_t height{};
};

struct Font {
    BLFont blFont{};
    float _ellipsisWidth{};

    auto ellipsisWidth() const noexcept { return this->_ellipsisWidth; }
    auto scaleX() const noexcept { return this->blFont.matrix().m00; }
    auto ascent() const noexcept { return this->blFont.metrics().ascent; }
    auto lineHeight() const noexcept {
        return this->blFont.metrics().ascent + this->blFont.metrics().descent + this->blFont.metrics().lineGap;
    }
    bool empty() const noexcept { return this->blFont.empty(); }
};

class FontFace final : public Resource {
 public:
    FontFace(const std::string& id);
    ~FontFace() override = default;

    static bool Equals(FontFace* fontFace, const std::string& family, StyleFontStyle style,
        StyleFontWeight weight) noexcept;

    void Load(Napi::Env env) override;
    Napi::Value Summarize(const Napi::Env& env) const override;

    const std::string& GetFamily() const;
    StyleFontStyle GetStyle() const noexcept;
    StyleFontWeight GetWeight() const noexcept;
    Font GetFont(float fontSize);

 private:
    AsyncWork<BLFontFace> work{};
    BLFontFace resource{};
    std::string family{};
    StyleFontStyle style{StyleFontStyleNormal};
    StyleFontWeight weight{StyleFontWeightNormal};
    int32_t index{0};
    BLGlyphBuffer ellipsis{};
    // LRU cache of fonts by size
    std::list<Font> fontsBySize{};
};

class Resources {
 public:
    bool HasImage(const std::string& path) const;
    Image* AcquireImage(const std::string& path);
    void ReleaseImage(const std::string& path, bool immediateDelete = false);

    bool HasFontFace(const std::string& path) const;
    FontFace* AcquireFontFace(const std::string& path);
    FontFace* AcquireFontFaceByStyle(const std::string& family, StyleFontStyle style, StyleFontWeight weight);
    void ReleaseFontFace(const std::string& path, bool immediateDelete = false);

    void ReleaseResource(Resource* resource, bool immediateDelete = false);

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

    using ImageDataRef = ResourceRef<Image>;
    using FontFaceRef = ResourceRef<FontFace>;

    using ImageDataMap = std::unordered_map<std::string, ImageDataRef>;
    using FontFaceMap = std::unordered_map<std::string, FontFaceRef>;

    ImageDataMap images;
    FontFaceMap fonts;
};

} // namespace ls
