/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <phmap.h>

namespace lse {

// Basic LRU cache.
template<typename K, typename V>
class LRUCache {
 public:
  using Function = void(*)(const K&, const V&);

  class ValueRef {
   public:
    ValueRef() = default;
    ValueRef(V* value) : value(value) {}

    V& Get() {
      return *(this->value);
    }

    bool Empty() {
      return this->value == nullptr;
    }

   private:
    V* value{};
  };

 public:
  LRUCache(std::size_t capacity) {
    this->table.reserve(capacity);
    this->storage = new Node[capacity]{};

    for (std::size_t i = 0; i < capacity; i++) {
      if (this->inactive) {
        auto node{&this->storage[i]};

        node->next = this->inactive;
        this->inactive->previous = node;
        this->inactive = node;
      } else {
        this->inactive = &this->storage[i];
      }
    }
  }

  ~LRUCache() {
    delete [] this->storage;
  }

  std::size_t Size() const {
    return this->table.size();
  }

  bool Has(const K& key) const {
    return this->table.contains(key);
  }

  ValueRef Find(const K& key) {
    auto p{this->table.find(key)};

    if (p == this->table.end()) {
      return {};
    }

    auto node{p->second};

    if (this->head != node) {
      if (node == this->tail) {
        this->RemoveTail();
      }

      if (node->previous) {
        node->previous->next = node->next;
      }

      if (node->next) {
        node->next->previous = node->previous;
      }

      node->previous = nullptr;
      node->next = this->head;

      if (this->head) {
        this->head->previous = node;
      }

      this->head = node;
    }

    return {&node->value};
  }

  bool Insert(const K& key, const V& value) {
    if (this->table.contains(key)) {
      return false;
    }

    Node* node;

    if (!this->inactive) {
      node = this->RemoveTail();
      this->table.erase(node->key);
    } else {
      node = this->inactive;
      this->inactive = node->next;
      node->previous = nullptr;
    }

    node->key = key;
    node->value = value;
    node->next = this->head;
    node->previous = nullptr;

    this->table[key] = node;

    if (this->head) {
      this->head->previous = node;
    }

    this->head = node;

    if (this->tail == nullptr) {
      this->tail = this->head;
    }

    return true;
  }

  void ForEach(Function func) {
    for (const auto& p : this->table) {
      func(p.first, p.second->value);
    }
  }

 private:
  struct Node {
    K key{};
    V value{};
    Node* previous{};
    Node* next{};
  };

  Node* RemoveTail() {
    auto node{this->tail};

    this->tail = this->tail->previous;

    if (this->tail) {
      this->tail->next = nullptr;
    } else {
      this->head = nullptr;
    }

    node->next = node->previous = nullptr;

    return node;
  }

 private:
  // doubly linked list with stable pointers.
  Node* head{};
  Node* tail{};
  Node* inactive{};
  Node* storage{};

  // key entries to doubly linked list nodes.
  phmap::flat_hash_map<K, Node*> table;
};

} // namespace lse
