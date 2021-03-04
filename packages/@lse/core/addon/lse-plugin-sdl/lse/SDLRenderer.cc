/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLRenderer.h>

#include <array>
#include <cstring>
#include <lse/SDLUtil.h>
#include <lse/PixelConversion.h>
#include <lse/Log.h>
#include <lse/string-ext.h>
#include <lse/math-ext.h>

namespace lse {

static const std::array<uint8_t, 4> kSinglePixelWhite{ 255, 255, 255, 255 };

class SDLTexture : public Texture {
 public:
  SDLTexture(std::shared_ptr<SDLRenderer> owner, SDL_Texture* texture,
      int32_t width, int32_t height, PixelFormat format, Type type) noexcept
  : Texture(std::move(owner), texture, width, height, format, type) {
  }

  bool Update(const uint8_t* pixels) noexcept override {
    if (!this->platformTexture) {
      return false;
    }

    auto pitch{this->width * 4};

    if (SDL2::SDL_UpdateTexture(this->As<SDL_Texture>(), nullptr, pixels, pitch) != 0) {
      LOG_ERROR(SDL2::SDL_GetError());
      return false;
    }

    return true;
  }

  uint8_t* Lock() noexcept override {
    if (!this->platformTexture) {
      return {};
    }

    void* pixels{};
    int32_t pitch{};

    if (SDL2::SDL_LockTexture(this->As<SDL_Texture>(), nullptr, &pixels, &pitch) != 0) {
      LOG_ERROR(SDL2::SDL_GetError());
      return {};
    }

    return static_cast<uint8_t*>(pixels);
  }

  void Unlock() noexcept override {
    if (this->platformTexture) {
      SDL2::SDL_UnlockTexture(this->As<SDL_Texture>());
    }
  }
};

SDLRenderer::SDLRenderer() {
  SDL_RendererInfo info;

  if (SDL2::SDL_GetRenderDriverInfo(0, &info) == 0) {
    this->UpdateTextureFormats(info);
  }

  this->floatMode = SDL2::SDL_RenderFillRectF != nullptr;
}

SDLRenderer::~SDLRenderer() {
  this->Destroy();
}

std::shared_ptr<SDLRenderer> SDLRenderer::New() {
  return std::make_shared<SDLRenderer>();
}

void SDLRenderer::UpdateTextureFormats(const SDL_RendererInfo& info) noexcept {
  constexpr static std::array<uint32_t, 4> supportedPixelFormats{
      SDL_PIXELFORMAT_ARGB8888,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_PIXELFORMAT_ABGR8888,
      SDL_PIXELFORMAT_BGRA8888
  };

  uint32_t format{ SDL_PIXELFORMAT_UNKNOWN };

  for (auto& p : supportedPixelFormats) {
    for (auto i{ 0u }; i < info.num_texture_formats; i++) {
      if (p == info.texture_formats[i]) {
        format = p;
        break;
      }
    }
  }

  this->textureFormat = ToPixelFormat(format);
}

void SDLRenderer::Present() noexcept {
  SDL2::SDL_RenderPresent(this->renderer);
}

void SDLRenderer::Clear(color_t color) noexcept {
  this->SetRenderDrawColor(color);
  SDL2::SDL_RenderClear(this->renderer);
}

bool SDLRenderer::SetRenderTarget(Texture* texture) noexcept {
  if (!texture || !texture->IsRenderTarget()) {
    LOG_ERROR("Invalid render target");
    return false;
  }

  if (SDL2::SDL_SetRenderTarget(this->renderer, texture->As<SDL_Texture>()) != 0) {
    LOG_ERROR(SDL2::SDL_GetError());
    return false;
  }

  this->ResetInternal();

  return true;
}

void SDLRenderer::Reset() noexcept {
  if (this->renderer) {
    SDL2::SDL_SetRenderTarget(this->renderer, nullptr);
    this->ResetInternal();
  }
}

void SDLRenderer::ResetInternal() {
  this->DisableClipping();
  this->SetRenderDrawColor(ColorWhite);
  SDL2::SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
}

void SDLRenderer::EnabledClipping(const Rect& rect) noexcept {
  const SDL_Rect clipRect{
    SnapToPixelGrid<int32_t>(rect.x),
    SnapToPixelGrid<int32_t>(rect.y),
    SnapToPixelGrid<int32_t>(rect.width),
    SnapToPixelGrid<int32_t>(rect.height)
  };

  SDL2::SDL_RenderSetClipRect(this->renderer, &clipRect);
}

void SDLRenderer::DisableClipping() noexcept {
  SDL2::SDL_RenderSetClipRect(this->renderer, nullptr);
}

Texture* SDLRenderer::CreateTexture(int32_t width, int32_t height, Texture::Type type) {
  auto sdlTexture{SDL2::SDL_CreateTexture(
      this->renderer,
      ToSDLPixelFormat(this->GetTextureFormat()),
      ToSDLTextureAccess(type),
      width,
      height)
  };

  if (!sdlTexture) {
    LOG_ERROR(SDL2::SDL_GetError());
    return {};
  }

  SDL2::SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);

  auto texture{new (std::nothrow) SDLTexture(
      this->shared_from_this(), sdlTexture, width, height, this->GetTextureFormat(), type)};

  if (texture) {
    this->textures.insert(texture);
  }

  return texture;
}

void SDLRenderer::DestroyTexture(Texture* texture) noexcept {
  if (!texture) {
    return;
  }

  if (!this->textures.contains(texture)) {
    LOG_ERROR("unknown texture");
    delete texture;
    return;
  }

  SDL2::SDL_DestroyTexture(texture->As<SDL_Texture>());
  this->textures.erase(texture);

  delete texture;
}

void SDLRenderer::SetRenderDrawColor(color_t color) noexcept {
  if (this->drawColor != color) {
    // TODO: consider opacity
    SDL2::SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    this->drawColor = color;
  }
}

void SDLRenderer::Attach(SDL_Window* window) {
  this->renderer = SDL2::SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!this->renderer) {
    throw std::runtime_error(Format("Failed to create an SDL renderer. SDL Error: %s", SDL2::SDL_GetError()));
  }

  SDL2::SDL_GetRendererOutputSize(this->renderer, &this->width, &this->height);

  SDL_RendererInfo info{};

  if (SDL2::SDL_GetRendererInfo(this->renderer, &info) == 0) {
    this->UpdateTextureFormats(info);
  }

  std::string textureFormats;

  for (auto i = 0u; i < info.num_texture_formats; i++) {
    if (!textureFormats.empty()) {
      textureFormats += ", ";
    }
    textureFormats += SDL2::SDL_GetPixelFormatName(info.texture_formats[i]);
  }

  LOGX_INFO("Texture Formats: %s", textureFormats);

  this->fillRectTexture = this->CreateTexture(1, 1, Texture::Updatable);

  if (!this->fillRectTexture) {
    throw std::runtime_error("Failed to create fill rect texture.");
  }

  this->fillRectTexture->Update(kSinglePixelWhite.data());

  LOGX_INFO("SDL_Renderer: %ix%i driver=%s renderer=%s textureFormat=%s maxTextureSize=%i,%i "
            "software=%s accelerated=%s vsync=%s renderTarget=%s",
            this->GetWidth(),
            this->GetHeight(),
            SDL2::SDL_GetCurrentVideoDriver(),
            info.name,
            SDL2::SDL_GetPixelFormatName(ToSDLPixelFormat(this->textureFormat)),
            info.max_texture_width,
            info.max_texture_height,
            (info.flags & SDL_RENDERER_SOFTWARE) != 0,
            (info.flags & SDL_RENDERER_ACCELERATED) != 0,
            (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0,
            (info.flags & SDL_RENDERER_TARGETTEXTURE) != 0);
}

void SDLRenderer::Detach() {
  this->fillRectTexture = Texture::SafeDestroy(this->fillRectTexture);

  if (!this->textures.empty()) {
    LOG_ERROR("leaked %i textures", this->textures.size());
  }

  this->renderer = DestroyRenderer(this->renderer);
  this->width = 0;
  this->height = 0;
}

void SDLRenderer::Destroy() {
  this->Detach();
}

void SDLRenderer::DrawImage(
    const RenderTransform& transform,
    const Point& origin,
    const Rect& box,
    const IntRect& src,
    Texture* texture,
    const RenderFilter& filter) noexcept {
  if (!texture) {
    return;
  }

  auto tex{texture->As<SDL_Texture>()};
  const auto& srcRect{reinterpret_cast<const SDL_Rect&>(src)};
  SDLSetTextureTint(tex, filter);

  if (this->floatMode) {
    auto destRect{SDLSnapToPixelGrid<SDL_FRect>(box)};

    SDL2::SDL_RenderCopyExF(
        this->renderer,
        tex,
        &srcRect,
        &destRect,
        transform.rotate,
        nullptr,
        SDLGetRenderFlip(filter));
  } else {
    auto destRect{SDLSnapToPixelGrid<SDL_Rect>(box)};

    SDL2::SDL_RenderCopyEx(
        this->renderer,
        tex,
        &srcRect,
        &destRect,
        transform.rotate,
        nullptr,
        SDLGetRenderFlip(filter));
  }
}

void SDLRenderer::DrawImage(
    const Rect& box, const IntRect& src, Texture* texture, const RenderFilter& filter) noexcept {
  if (!texture) {
    return;
  }

  auto tex{texture->As<SDL_Texture>()};
  const auto& srcRect{reinterpret_cast<const SDL_Rect&>(src)};
  SDLSetTextureTint(tex, filter);

  if (this->floatMode) {
    auto destRect{SDLSnapToPixelGrid<SDL_FRect>(box)};

    if (filter.HasFlip()) {
      SDL2::SDL_RenderCopyExF(this->renderer, tex, &srcRect, &destRect, 0, {}, SDLGetRenderFlip(filter));
    } else {
      SDL2::SDL_RenderCopyF(this->renderer, tex, &srcRect, &destRect);
    }
  } else {
    auto destRect{SDLSnapToPixelGrid<SDL_Rect>(box)};

    if (filter.HasFlip()) {
      SDL2::SDL_RenderCopyEx(this->renderer, tex, &srcRect, &destRect, 0, {}, SDLGetRenderFlip(filter));
    } else {
      SDL2::SDL_RenderCopy(this->renderer, tex, &srcRect, &destRect);
    }
  }
}

static void LayoutCapInsetsSourceRects(const EdgeRect& capInsets, Texture* texture, SDL_Rect* src) noexcept {
  const auto top{capInsets.top};
  const auto right{capInsets.right};
  const auto bottom{capInsets.bottom};
  const auto left{capInsets.left};

  const auto textureWidth{texture->Width()};
  const auto textureHeight{texture->Height()};

  // Top row
  src[0] = { 0, 0, left, top };
  src[1] = { left, 0, textureWidth - left - right, top };
  src[2] = { textureWidth - right, 0, right, top };
  // Middle row
  src[3] = { 0, top, left, textureHeight - top - bottom };
  src[4] = { left, top, textureWidth - left - right, textureHeight - top - bottom };
  src[5] = { textureWidth - right, top, right, textureHeight - top - bottom };
  // Bottom row
  src[6] = { 0, textureHeight - bottom, left, bottom };
  src[7] = { left, textureHeight - bottom, textureWidth - left - right, bottom };
  src[8] = { textureWidth - right, textureHeight - bottom, right, bottom };
}

template<typename R, typename D>
static void LayoutCapInsetsDestRects(const Rect& box, const EdgeRect& capInsets, R* rects) noexcept {
  const auto x{ SnapToPixelGrid<D>(box.x) };
  const auto y{ SnapToPixelGrid<D>(box.y) };
  const auto w{ SnapToPixelGrid<D>(box.width) };
  const auto h{ SnapToPixelGrid<D>(box.height) };

  const auto t{static_cast<D>(capInsets.top)};
  const auto r{static_cast<D>(capInsets.right)};
  const auto b{static_cast<D>(capInsets.bottom)};
  const auto l{static_cast<D>(capInsets.left)};

  // Top row
  rects[0] = { x, y, l, t };
  rects[1] = { x + l, y, w - l - r, t };
  rects[2] = { x + w - r, y, r, t };
  // Middle row
  rects[3] = { x, y + t, l, h - t - b };
  rects[4] = { x + l, y + t, w - l - r, h - t - b };
  rects[5] = { x + w - r, y + t, r, h - t - b };
  // Bottom row
  rects[6] = { x, y + h - b, l, b };
  rects[7] = { x + l, y + h - b, w - l - r, b };
  rects[8] = { x + w - r, y + h - b, r, b };
}

void SDLRenderer::DrawImageCapInsets(
    const Rect& box, const EdgeRect& capInsets, Texture* texture, const RenderFilter& filter) noexcept {
  if (!texture) {
    return;
  }

  static constexpr auto kSize = 9;
  SDL_Rect src[kSize];
  auto nativeTexture{texture->As<SDL_Texture>()};

  SDLSetTextureTint(nativeTexture, filter);
  LayoutCapInsetsSourceRects(capInsets, texture, src);

  if (this->floatMode) {
    SDL_FRect dest[kSize];

    LayoutCapInsetsDestRects<SDL_FRect, float>(box, capInsets, dest);

    for (auto i = 0; i < kSize; i++) {
      SDL2::SDL_RenderCopyF(this->renderer, nativeTexture, &src[i], &dest[i]);
    }
  } else {
    SDL_Rect dest[kSize];

    LayoutCapInsetsDestRects<SDL_Rect, int32_t>(box, capInsets, dest);

    for (auto i = 0; i < kSize; i++) {
      SDL2::SDL_RenderCopy(this->renderer, nativeTexture, &src[i], &dest[i]);
    }
  }
}

void SDLRenderer::FillRect(const Rect& box, const RenderFilter& filter) noexcept {
  SDLSetDrawColor(this->renderer, filter);

  if (this->floatMode) {
    auto dest{SDLSnapToPixelGrid<SDL_FRect>(box)};
    SDL2::SDL_RenderFillRectF(this->renderer, &dest);
  } else {
    auto dest{SDLSnapToPixelGrid<SDL_Rect>(box)};
    SDL2::SDL_RenderFillRect(this->renderer, &dest);
  }
}

template<typename R, typename D>
static int32_t LayoutBorder(const Rect& box, const EdgeRect& edges, R* dest) {
  const auto x = SnapToPixelGrid<D>(box.x);
  const auto y = SnapToPixelGrid<D>(box.y);
  const auto w = SnapToPixelGrid<D>(box.width);
  const auto h = SnapToPixelGrid<D>(box.height);

  const auto top = static_cast<D>(edges.top);
  const auto right = static_cast<D>(edges.right);
  const auto bottom = static_cast<D>(edges.bottom);
  const auto left = static_cast<D>(edges.left);

  int32_t count = 0;

  if (top > 0) {
    dest[count++] = { x, y, w, top };
  }

  if (right > 0) {
    dest[count++] = { x + w - right, y + top, right, h - top - bottom };
  }

  if (bottom > 0) {
    dest[count++] = { x, y + h - bottom, w, bottom };
  }

  if (left > 0) {
    dest[count++] = { x, y + top, left, h - top - bottom };
  }

  return count;
}

void SDLRenderer::StrokeRect(const Rect& box, const EdgeRect& edges, const RenderFilter& filter) noexcept {
  SDLSetDrawColor(this->renderer, filter);

  if (this->floatMode) {
    SDL_FRect dest[4];
    int32_t count{LayoutBorder<SDL_FRect, float>(box, edges, dest)};

    SDL2::SDL_RenderFillRectsF(this->renderer, dest, count);
  } else {
    SDL_Rect dest[4];
    int32_t count{LayoutBorder<SDL_Rect, int32_t>(box, edges, dest)};

    SDL2::SDL_RenderFillRects(this->renderer, dest, count);
  }
}

} // namespace lse
