---
marp: true
theme: default
paginate: true
size: 16:9
---

# Allocators for C++ vocabulary types

_Jonathan B. Coe & Antony Peacock_

---

## Outline

---

## Our new type: `dyn_optional`

We'll work on a new type, `dyn_optional`, for our examples.

`dyn_optional`, like `std::optional`, is a type that can hold a value or be empty.

When `dyn_optional` is non-empty, the value is stored in dynamic memory.

There are not a host of good reasons to use `dyn_optional` but it should be simple enough for our examples.

---

## Class definition

```cpp
template <typename T>
class dyn_optional {
private:
    T* ptr;
  public:
    // Constructors
    dyn_optional() noexcept;
    dyn_optional(T const& value);

    // Copy and move constructors
    dyn_optional(const dyn_optional& other);
    dyn_optional(dyn_optional&& other);

    // Assignment operators
    dyn_optional& operator=(const dyn_optional& other);
    dyn_optional& operator=(dyn_optional&& other);

    ...
```

---

## Class definition (continued)

```cpp
    ...

    // Observers
    operator bool() const noexcept;

    // Const accessors
    T const* operator->() const;
    T const& operator*() const;

    // Non-const accessors
    T* operator->();
    T& operator*();

    // Modifiers
    void reset();
    void swap(dyn_optional& other);

    // Destructor
    ~dyn_optional();
};
```

---

### Constructors

```cpp
template <typename T>
dyn_optional<T>::dyn_optional() noexcept
    : ptr(nullptr) {}
```

```cpp
template <typename T>
dyn_optional<T>::dyn_optional(const T& t) noexcept
    : ptr(new T(t)) {}
```

---

## Copy and Move Constructors

```cpp
template <typename T>
dyn_optional<T>::dyn_optional(const dyn_optional<T>& other) noexcept {
    if (other.ptr) {
        ptr = new T(*other.ptr);
    } else {
        ptr = nullptr;
    }
}
```

```cpp
template <typename T>
dyn_optional<T>::dyn_optional(dyn_optional<T>&& other) noexcept {
    ptr = other.ptr;
    other.ptr = nullptr;
}
```

## Assignment Operators

```cpp
template <typename T>
dyn_optional<T>& dyn_optional<T>::operator=(const dyn_optional<T>& other) {
    if(this==&other) return *this;
    delete ptr;
    ptr = other.ptr ? new T(*other.ptr) : nullptr;
    return *this;
}
```

```cpp
template <typename T>
dyn_optional<T>& dyn_optional<T>::operator=(dyn_optional<T>&& other) {
    if(this==&other) return *this;
    delete ptr;
    ptr = nullptr;
    std::swap(ptr, other.ptr);
    return *this;
}
```

## Observers

```cpp
template <typename T>
dyn_optional<T>::operator bool() const noexcept {
    return ptr != nullptr;
}
```

---

## Const Accessors

```cpp
template <typename T>
T const* dyn_optional<T>::operator->() const {
    return ptr;
}
```

```cpp
template <typename T>
T const& dyn_optional<T>::operator*() const {
    return *ptr;
}
```

---

## Non-const Accessors

```cpp
template <typename T>
T* dyn_optional<T>::operator->() {
    return ptr;
}
```

```cpp
template <typename T>
T& dyn_optional<T>::operator*() {
    return *ptr;
}
```

---

## Modifiers

```cpp
template <typename T>
void dyn_optional<T>::swap(dyn_optional<T>& other) {
    std::swap(ptr, other.ptr);
}
```

---

## Destructor

```cpp
template <typename T>
dyn_optional<T>::~dyn_optional() {
    delete ptr;
}
```

---

## Allocating memory

We've used `new` to allocate memory for the value in `dyn_optional` and `delete` to deallocate it.

This seems neat enough but I've heard talk of things called allocators.

Allocators allow us to customize object creation and destruction, and memory allocation and deallocation.

---

## Allocators: The Hawaiian Lego Analogy

TODO

---

## Adding an allocator to dyn_optional

We could make an allocator part of `dyn_optional` and use the allocator to allocate and deallocate memory.

A user could then specify the allocator they want to use when they create a `dyn_optional`.

We add the allocator as a second template parameter to `dyn_optional`. By default we use `std::allocator` so that if a
user doesn't specify an allocator, we use the standard allocator just like other standard library types.

```cpp
template <typename T, typename Allocator = std::allocator<T>>
class dyn_optional;
```

---

## Propagating allocators

---

## Scoped allocators

---

## Allocator-extended constructors

---

## Allocators and C++ vocabulary types

---
