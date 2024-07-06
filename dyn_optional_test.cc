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

#include "dyn_optional.h"

#include <memory>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"

namespace {

using xyz::dyn_optional;

template <typename T>
class PropagatingAllocator {
 public:
  using value_type = T;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  inline static size_t default_construct_count = 0;

  PropagatingAllocator() { ++default_construct_count; }

  T* allocate(std::size_t n) const {
    std::allocator<T> default_allocator{};
    return default_allocator.allocate(n);
  }

  void deallocate(T* p, std::size_t n) const {
    std::allocator<T> default_allocator{};
    return default_allocator.deallocate(p, n);
  }
};

template <typename T>
class NonPropagatingAllocator {
 public:
  using value_type = T;
  using propagate_on_container_copy_assignment = std::false_type;
  using propagate_on_container_move_assignment = std::false_type;
  using propagate_on_container_swap = std::false_type;

  inline static size_t default_construct_count = 0;

  NonPropagatingAllocator() { ++default_construct_count; }

  NonPropagatingAllocator select_on_container_copy_construction() const {
    return NonPropagatingAllocator();
  }

  T* allocate(std::size_t n) const {
    std::allocator<T> default_allocator{};
    return default_allocator.allocate(n);
  }

  void deallocate(T* p, std::size_t n) const {
    std::allocator<T> default_allocator{};
    return default_allocator.deallocate(p, n);
  }
};

TEST(TestUtilities, PropagatingAllocator) {
  using allocator_traits = std::allocator_traits<PropagatingAllocator<int>>;
  EXPECT_TRUE(allocator_traits::propagate_on_container_copy_assignment::value);
  EXPECT_TRUE(allocator_traits::propagate_on_container_move_assignment::value);
  EXPECT_TRUE(allocator_traits::propagate_on_container_swap::value);

  auto a = PropagatingAllocator<int>{};
  [[maybe_unused]] auto aa =
      allocator_traits::select_on_container_copy_construction(a);
  EXPECT_EQ(PropagatingAllocator<int>::default_construct_count, 1);
}

TEST(TestUtilities, NonPropagatingAllocator) {
  using allocator_traits = std::allocator_traits<NonPropagatingAllocator<int>>;
  EXPECT_FALSE(allocator_traits::propagate_on_container_copy_assignment::value);
  EXPECT_FALSE(allocator_traits::propagate_on_container_move_assignment::value);
  EXPECT_FALSE(allocator_traits::propagate_on_container_swap::value);

  auto a = NonPropagatingAllocator<int>{};
  [[maybe_unused]] auto aa =
      allocator_traits::select_on_container_copy_construction(a);
  EXPECT_EQ(NonPropagatingAllocator<int>::default_construct_count, 2);
}

TEST(DynOptionalDefaultAllocator, DefaultConstruct) {
  dyn_optional<int> opt;
  EXPECT_FALSE(opt);
}

TEST(DynOptionalPropagatingAllocator, DefaultConstruct) {
  dyn_optional<int, NonPropagatingAllocator<int>> opt;
  EXPECT_FALSE(opt);
}

TEST(DynOptionalNonPropagatingAllocator, DefaultConstruct) {
  dyn_optional<int, NonPropagatingAllocator<int>> opt;
  EXPECT_FALSE(opt);
}

TEST(DynOptionalDefaultAllocator, ValueConstruct) {
  dyn_optional<int> opt(42);
  EXPECT_TRUE(opt);
  EXPECT_EQ(*opt, 42);
}

TEST(DynOptionalPropagatingAllocator, ValueConstruct) {
  dyn_optional<int, NonPropagatingAllocator<int>> opt(42);
  EXPECT_TRUE(opt);
  EXPECT_EQ(*opt, 42);
}

TEST(DynOptionalNonPropagatingAllocator, ValueConstruct) {
  dyn_optional<int, NonPropagatingAllocator<int>> opt(42);
  EXPECT_TRUE(opt);
  EXPECT_EQ(*opt, 42);
}
}  // namespace
