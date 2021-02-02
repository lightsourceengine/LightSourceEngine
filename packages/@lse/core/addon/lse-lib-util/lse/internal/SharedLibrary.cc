/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/internal/SharedLibrary.h>

#include <stdexcept>
#include <lse/string-ext.h>
#include <lse/Log.h>

#if defined(_WIN32)
#include <windows.h>

#define SharedLibraryOpen(NAME) LoadLibrary((NAME))
#define SharedLibraryClose(HANDLE) FreeLibrary(reinterpret_cast<HMODULE>(HANDLE))
#define SharedLibraryGetLastError() GetLastError()
#define SharedLibraryGetSymbol(HANDLE, SYMBOL) GetProcAddress(reinterpret_cast<HMODULE>(HANDLE), (SYMBOL))
#else
#include <dlfcn.h>

#define SharedLibraryOpen(NAME) dlopen((NAME), RTLD_LAZY)
#define SharedLibraryClose(HANDLE) dlclose(HANDLE)
#define SharedLibraryGetLastError() dlerror()
#define SharedLibraryGetSymbol(HANDLE, SYMBOL) dlsym((HANDLE), (SYMBOL))
#endif

namespace lse {
namespace internal {

void SharedLibrary::Open(const char* libraryName) {
  LOG_INFO(libraryName);

  if (this->handle != nullptr) {
    throw std::runtime_error(Format("%s has already been opened.", libraryName));
  }

  this->handle = SharedLibraryOpen(libraryName);

  if (!this->handle) {
    throw std::runtime_error(Format("Error loading %s: %s", libraryName, SharedLibraryGetLastError()));
  }

  this->refs++;
}

void* SharedLibrary::GetSymbol(const char* name, bool required) const {
  if (!this->handle) {
    throw std::runtime_error("Cannot get symbol of unopened library.");
  }

  auto symbol = SharedLibraryGetSymbol(this->handle, name);

  if (required && symbol == nullptr) {
    throw std::runtime_error(Format("Error retrieving symbol %s", name));
  }

  return symbol;
}

bool SharedLibrary::IsOpen() const noexcept {
  return this->handle != nullptr;
}

void SharedLibrary::Close() noexcept {
  if (--this->refs == 0 && this->handle) {
    SharedLibraryClose(this->handle);
  }
}

int32_t SharedLibrary::Ref() noexcept {
  return ++this->refs;
}

} // namespace internal
} // namespace lse

#undef SharedLibraryOpen
#undef SharedLibraryClose
#undef SharedLibraryGetLastError
#undef SharedLibraryGetSymbol
