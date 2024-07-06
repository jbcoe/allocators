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

#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
namespace xyz {

#ifndef XYZ_UNREACHABLE_DEFINED
#define XYZ_UNREACHABLE_DEFINED
[[noreturn]] inline void unreachable() {  // LCOV_EXCL_LINE
#if (__cpp_lib_unreachable >= 202202L)
  std::unreachable();  // LCOV_EXCL_LINE
#elif defined(_MSC_VER)
  __assume(false);  // LCOV_EXCL_LINE
#else
  __builtin_unreachable();  // LCOV_EXCL_LINE
#endif
}
#endif  // XYZ_UNREACHABLE_DEFINED
template <typename T, typename Allocator = std::allocator<T>>
class dyn_optional {
  static_assert(sizeof(T), "T cannot be an incomplete type");
  static_assert(std::is_object_v<T> && !std::is_array_v<T>);

 public:
  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<Allocator>;
  using pointer = std::allocator_traits<Allocator>::pointer;

  template <typename... Us>
  constexpr static pointer construct_from(Allocator a, Us&&... us) {
    pointer p = allocator_traits::allocate(a, 1);
    try {
      allocator_traits::construct(a, std::to_address(p),
                                  std::forward<Us>(us)...);
      return p;
    } catch (...) {
      allocator_traits::deallocate(a, p, 1);
      throw;
    }
  }

  template <typename... Us>
  constexpr static void delete_with(Allocator a, pointer p) noexcept {
    if (p == nullptr) return;
    allocator_traits::destroy(a, std::to_address(p));
    allocator_traits::deallocate(a, p, 1);
  }

 private:
  [[no_unique_address]] Allocator allocator_;
  T* ptr_;

 public:
  // Constructors
  constexpr dyn_optional()
    requires std::is_default_constructible_v<Allocator>
      : allocator_(), ptr_(nullptr) {}

  template <typename... Us>
  constexpr dyn_optional(Us&&... us)
    requires std::is_default_constructible_v<Allocator> &&
                 std::is_constructible_v<T, Us...>
      : allocator_(),
        ptr_(construct_from(allocator_, std::forward<Us>(us)...)) {}

  // Allocator-extended constructors
  constexpr dyn_optional(std::allocator_arg_t, Allocator a)
      : allocator_(a), ptr_(nullptr) {}

  template <typename... Us>
  constexpr dyn_optional(std::allocator_arg_t, Allocator a, Us&&... us)
    requires std::is_constructible_v<T, Us...>
      : allocator_(a),
        ptr_(construct_from(allocator_, std::forward<Us>(us)...)) {}

  // Copy and Move Constructor
  constexpr dyn_optional(const dyn_optional& other)
    requires std::is_copy_constructible_v<T>
      : allocator_(allocator_traits::select_on_container_copy_construction(
            other.allocator_)),
        ptr_(other.ptr_ ? construct_from(allocator_, *other.ptr_) : nullptr) {}

  constexpr dyn_optional(dyn_optional&& other)
    requires std::is_move_constructible_v<T>
      : allocator_(other.allocator_), ptr_(other.ptr_) {
    other.ptr = nullptr;
  }

  // Allocator-extended Copy and Move Constructor
  constexpr dyn_optional(std::allocator_arg_t, Allocator a,
                         const dyn_optional& other)
    requires std::is_copy_constructible_v<T>
      : allocator_(allocator_traits::select_on_container_copy_construction(a)),
        ptr_(other.ptr_ ? construct_from(allocator_, *other.ptr) : nullptr) {}

  constexpr dyn_optional(std::allocator_arg_t, Allocator a,
                         dyn_optional&& other)

    requires std::is_move_constructible_v<T>
      : allocator_(a), ptr_(other.ptr_) {
    other.ptr = nullptr;
  }

  // Copy and Move Assignment
  constexpr dyn_optional& operator=(const dyn_optional& other)
    requires std::is_copy_constructible_v<T>
  {
    if (this != &other) {
      const bool POCCA =
          allocator_traits::propagate_on_container_copy_assignment::value;
      if (!other) {
        if (ptr_) destroy(allocator_, ptr_);
      } else {
        pointer tmp =
            construct_from(POCCA ? other.allocator_ : allocator_, *other.ptr_);
        if (ptr_) destroy(allocator_, ptr_);
        ptr_ = tmp;
      }
      if (POCCA) {
        allocator_ = other.allocator_;
      }
    }
    return *this;
  }

  constexpr dyn_optional& operator=(dyn_optional&& other) noexcept(
      allocator_traits::is_always_equal::value)
    requires allocator_traits::is_always_equal::value
             || std::is_move_constructible_v<T>
  {
    if (this != &other) {
      const bool POCMA =
          allocator_traits::propagate_on_container_move_assignment::value;
      if (!other) {
        if (ptr_) destroy(allocator_, ptr_);
      } else {
        if (allocator_ == other.allocator_) {
          using namespace std;
          swap(ptr_, other.ptr_);
          destroy(other.allocator_, other.ptr_);
        } else {
          pointer tmp = construct_from(POCMA ? other.allocator_ : allocator_,
                                       std::move(*other.ptr_));
          if (ptr_) destroy(allocator_, ptr_);
          ptr_ = tmp;
        }
      }
      if (POCMA) {
        allocator_ = other.allocator_;
      }
    }
    return *this;
  }

  // Observers
  constexpr operator bool() const noexcept { return ptr_ != nullptr; }

  // Const accessors
  constexpr const T& operator*() const noexcept {
    assert(ptr_);
    return *std::to_address(ptr_);
  }

  constexpr const T* operator->() const noexcept {
    assert(ptr_);
    return std::to_address(ptr_);
  }

  // Non-const accessors
  constexpr T& operator*() noexcept {
    assert(ptr_);
    return *std::to_address(ptr_);
  }

  constexpr T* operator->() noexcept {
    assert(ptr_);
    return std::to_address(ptr_);
  }

  // Modifiers
  constexpr void swap(dyn_optional& other) noexcept(
      allocator_traits::propagate_on_container_swap::value ||
      allocator_traits::is_always_equal::value) {
    using std::swap;
    if constexpr (allocator_traits::propagate_on_container_swap::value) {
      swap(allocator_, other.allocator_);
      swap(ptr_, other.ptr_);
      return;
    } else /* constexpr */ {
      if (allocator_ == other.allocator_) {
        swap(ptr_, other.ptr_);
      } else {
        unreachable();
      }
    }
  }

  friend constexpr void swap(dyn_optional& lhs, dyn_optional& rhs) noexcept(
      allocator_traits::propagate_on_container_swap::value ||
      allocator_traits::is_always_equal::value) {
    lhs.swap(rhs);
  }

  constexpr void reset() noexcept {
    delete_with(allocator_, ptr_);
    ptr_ = nullptr;
  }

  // Destructor
  constexpr ~dyn_optional() /* implicitly noexcept */ { reset(); }
};

}  // namespace xyz
