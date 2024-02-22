/* Copyright (c) 2024 The XYZ Authors. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
==============================================================================*/

#include <gtest/gtest.h>

namespace {
TEST(StandardAllocator, AllocateAndDeallocateMemory) {
  std::allocator<int> allocator;
  using allocator_traits = std::allocator_traits<decltype(allocator)>;
  using pointer = typename allocator_traits::pointer;

  pointer mem = allocator_traits::allocate(allocator, 1);
  EXPECT_NE(mem, nullptr);

  allocator_traits::deallocate(allocator, mem, 1);
}

TEST(StandardAllocator, ConstructAndDestroyAnObject) {
  std::allocator<int> allocator;
  using allocator_traits = std::allocator_traits<decltype(allocator)>;
  using pointer = typename allocator_traits::pointer;

  pointer mem = allocator_traits::allocate(allocator, 1);

  allocator_traits::construct(allocator, mem, 42);
  EXPECT_EQ(*mem, 42);
  allocator_traits::destroy(allocator, mem);

  allocator_traits::deallocate(allocator, mem, 1);
}

struct Counters {
  size_t allocation_count = 0;
  size_t deallocation_count = 0;
  size_t construction_count = 0;
  size_t destruction_count = 0;
};

template <typename T>
struct CustomAllocator {
  Counters* counters;

  using value_type = T;
  using pointer = T*;

  CustomAllocator(Counters* counters) : counters(counters) {}

  pointer allocate(std::size_t n) {
    ++counters->allocation_count;
    return new T[n];
  }
  void deallocate(pointer p, std::size_t n) {
    ++counters->deallocation_count;
    delete[] p;
  }

  template <class U, class... Args>
  void construct(U* p, Args&&... args) {
    ++counters->construction_count;
    new (p) U(std::forward<Args>(args)...);
  }

  template <class U>
  void destroy(U* p) {
    ++counters->destruction_count;
    p->~U();
  }
};

TEST(CustomAllocator, AllocateAndDeallocateMemory) {
  Counters counters;
  CustomAllocator<int> allocator(&counters);
  using allocator_traits = std::allocator_traits<decltype(allocator)>;
  using pointer = typename allocator_traits::pointer;

  pointer mem = allocator_traits::allocate(allocator, 1);
  EXPECT_NE(mem, nullptr);

  allocator_traits::deallocate(allocator, mem, 1);

  EXPECT_EQ(counters.allocation_count, 1);
  EXPECT_EQ(counters.deallocation_count, 1);
}

TEST(CustomAllocator, ConstructAndDestroyAnObject) {
  Counters counters;
  CustomAllocator<int> allocator(&counters);
  using allocator_traits = std::allocator_traits<decltype(allocator)>;
  using pointer = typename allocator_traits::pointer;

  pointer mem = allocator_traits::allocate(allocator, 1);

  allocator_traits::construct(allocator, mem, 42);
  EXPECT_EQ(*mem, 42);
  allocator_traits::destroy(allocator, mem);

  allocator_traits::deallocate(allocator, mem, 1);

  EXPECT_EQ(counters.allocation_count, 1);
  EXPECT_EQ(counters.deallocation_count, 1);
  EXPECT_EQ(counters.construction_count, 1);
  EXPECT_EQ(counters.destruction_count, 1);
}

}  // namespace
