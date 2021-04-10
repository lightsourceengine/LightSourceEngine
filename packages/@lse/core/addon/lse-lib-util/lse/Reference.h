/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <cstdint>
#include <cstddef>

namespace lse {

/**
 * Simple reference counting interface for LSE native objects.
 *
 * The std smart pointers and the napi references require a bit too much overhead
 * to manage object lifetimes in LSE. This implementation is simple and it seems to work.
 *
 * Native objects that are passed around extend this interface. User of the object use
 * ReferenceHolder tp manage their reference.
 */
class Reference {
 public:
  virtual ~Reference() = default;

  void Ref() noexcept {
    this->refs++;
  }

  void Unref() noexcept {
    if (--this->refs == 0) {
      delete this;
    }
  }

  int32_t RefCount() const noexcept {
    return this->refs;
  }

 private:
  int32_t refs{1};
};

/**
 * Manages ref'ing and unref'ing a Reference object.
 *
 * ReferenceHolders are not mean to be passed around the system. They are used
 * inside of a class to manage a Reference.
 *
 * @tparam T a Reference class
 */
template<class T>
class ReferenceHolder {
//  static_assert(std::is_base_of<Reference, T>::value, "T must inherit from Reference");

 public:
  // constructors
  ReferenceHolder() = default;
  inline ReferenceHolder(T* reference);
  ReferenceHolder(const ReferenceHolder<T>&) = delete;

  // destructor
  inline ~ReferenceHolder();

  // assignment
  inline ReferenceHolder<T>& operator=(std::nullptr_t) noexcept;
  inline ReferenceHolder<T>& operator=(T* ref) noexcept;
  ReferenceHolder<T>& operator=(ReferenceHolder<T>&& other) = delete;
  void operator=(const ReferenceHolder<T>&) = delete;

  // observers
  T* Get() const noexcept { return this->reference; }
  T* operator->() const noexcept { return this->reference; }
  explicit operator bool() const noexcept { return this->reference != nullptr; }
  operator T*() const noexcept { return this->reference; }

 private:
  inline void Release() noexcept;
  inline void Acquire(T* ref) noexcept;

 private:
  T* reference{};
};

template<class T>
inline ReferenceHolder<T>::ReferenceHolder(T* reference) {
  this->Acquire(reference);
}

template<class T>
inline ReferenceHolder<T>::~ReferenceHolder() {
  this->Release();
}

template<class T>
inline ReferenceHolder<T>& ReferenceHolder<T>::operator=(std::nullptr_t) noexcept {
  this->Release(); return *this;
}

template<class T>
inline ReferenceHolder<T>& ReferenceHolder<T>::operator=(T* ref) noexcept {
  this->Acquire(ref); return *this;
}

template<class T>
inline void ReferenceHolder<T>::Release() noexcept {
  if (this->reference) {
    this->reference->Unref();
    this->reference = nullptr;
  }
}

template<class T>
inline void ReferenceHolder<T>::Acquire(T* ref) noexcept {
  this->Release();
  if (ref) {
    this->reference = ref;
    this->reference->Ref();
  }
}

} // namespace lse
