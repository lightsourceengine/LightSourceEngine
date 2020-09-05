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
#include <ls/types.h>

#include <memory>
#include <string>
#include <list>
#include <phmap.h>
#include <std17/filesystem>

namespace ls {

class Renderer;

class Resource {
 public:
    using Owner = void*;
    using Listener = std::function<void(Owner, Resource*)>;
    using ResourceId = uint32_t;

    enum State {
        Init,
        Ready,
        Error,
        Loading,
    };

 public:
    Resource(const std::string& tag);
    virtual ~Resource() = default;

    ResourceId GetId() const noexcept { return this->id; }
    const std::string& GetTag() const noexcept { return this->tag; }

    void AddListener(Owner owner, Listener&& listener);
    void RemoveListener(Owner owner);
    State GetState() const noexcept { return this->state; }
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

    static ResourceId nextResourceId;

    ResourceId id;
    std::string tag{};
    std17::filesystem::path path{};
    std::vector<ListenerEntry> listeners{};
    State state{ Init };
    std::string errorMessage{};

    friend class Resources;
};

class Image final : public Resource {
 public:
    Image(const std::string& tag) : Resource(tag) {}
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
    static Image Mock(const std::string& tag, int32_t width, int32_t height);

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
    FontFace(const std::string& tag);
    ~FontFace() override = default;

    static bool Equals(const FontFaceRef& fontFace, const std::string& family, StyleFontStyle style,
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
    bool HasImage(const std::string& tag) const;
    ImageRef AcquireImage(const std::string& tag);

    bool HasFontFace(const std::string& tag) const;
    FontFaceRef AcquireFontFace(const std::string& tag);
    FontFaceRef AcquireFontFaceByStyle(const std::string& family, StyleFontStyle style, StyleFontWeight weight);

    void ReleaseResource(Resource* resource, bool immediateDelete = false);

    void Compact();

 private:
    using ImageDataMap = phmap::flat_hash_map<std::string, ImageRef>;
    using FontFaceMap = phmap::flat_hash_map<std::string, FontFaceRef>;

    ImageDataMap images;
    FontFaceMap fonts;
    phmap::flat_hash_set<Resource*> pendingDeletions;
};

} // namespace ls
