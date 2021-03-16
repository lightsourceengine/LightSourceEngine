/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <lse/LRUCache.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void LRUCacheSpec(TestSuite* parent) {
  const auto spec{parent->Describe("LRUCache")};

  spec->Describe("constructor")->tests = {
    {
      "should create an empty LRUCache with capacity of 5",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(5);

        Assert::Equal(lru.Size(), 0u);
      }
    }
  };

  spec->Describe("Has()")->tests = {
    {
      "should return true if entry with key exists",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        lru.Insert(1, 101);

        Assert::IsTrue(lru.Has(1));
      }
    },
    {
      "should return false if entry does not exist",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        Assert::IsFalse(lru.Has(1));
      }
    }
  };

  spec->Describe("Find()")->tests = {
    {
      "should find value for key",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        lru.Insert(1, 101);

        auto ref{lru.Find(1)};

        Assert::IsFalse(ref.Empty());
        Assert::Equal(ref.Get(), 101);
      }
    },
    {
      "should return empty ref if item does not exist",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        Assert::IsTrue(lru.Find(1).Empty());
      }
    }
  };

  spec->Describe("Insert()")->tests = {
    {
      "should insert items into the cache",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        Assert::IsTrue(lru.Insert(1, 101));
        Assert::IsTrue(lru.Insert(2, 102));
        Assert::IsTrue(lru.Insert(3, 103));

        Assert::Equal(lru.Size(), 3u);
      }
    },
    {
      "should insert new item and remove oldest to make space (v1)",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        Assert::IsTrue(lru.Insert(1, 101));
        Assert::IsTrue(lru.Insert(2, 102));
        Assert::IsTrue(lru.Insert(3, 103));

        // insert item, removing key = 1 to make space
        Assert::IsTrue(lru.Insert(4, 104));

        Assert::Equal(lru.Size(), 3u);
        lru.ForEach([](const int32_t& key, const int32_t& value) {
          Assert::IsTrue(key != 1);
        });
      }
    },
    {
      "should insert new item and remove oldest to make space (v2)",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        // insert items. key = 1 is the oldest item in the cache.
        Assert::IsTrue(lru.Insert(1, 101));
        Assert::IsTrue(lru.Insert(2, 102));
        Assert::IsTrue(lru.Insert(3, 103));

        // touch key = 1, so key = 2 is now the oldest item in the cache.
        lru.Find(1);

        // insert a new item, removing the oldest item (key = 2) from the cache.
        Assert::IsTrue(lru.Insert(4, 104));

        Assert::Equal(lru.Size(), 3u);
        lru.ForEach([](const int32_t& key, const int32_t& value) {
          Assert::IsTrue(key != 2);
        });
      }
    },
    {
      "should return false if key already exists",
      [](const TestInfo&) {
        LRUCache<int32_t, int32_t> lru(3);

        Assert::IsTrue(lru.Insert(1, 101));
        Assert::IsFalse(lru.Insert(1, 101));
        Assert::Equal(lru.Size(), 1u);
      }
    }
  };
}

} // namespace lse
