---
marp: true
theme: default
paginate: true
size: 16:9
---

# Allocators for C++ vocabulary types

<i>Jonathan B. Coe & Antony Peacock</i>

---

## Outline

1. Creating a new type: `dyn_optional`.
1. Implementing our new type.
1. Introducing allocators.
1. Allocator traits.
1. Allocator propagation.
1. Why `dyn_optional` needs an allocator.
1. Adding an allocator to `dyn_optional`.

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
    template <typename ...Us> dyn_optional(Us&& ...us);

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
template <typename T, typename ...Us>
dyn_optional<T>::dyn_optional(Us&& ...us);
    : ptr(new T(std::forward<Us>(us)...)) {}
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
---

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

---

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

## C++03 allocators [Bob's slide]

Pointers were always `T*`.

Implementations assumed that allocators were stateless as instances always compared equal and could be considered interchangeable.

Shared memory data structures could not be readily implemented with standard library containers.

Scoped allocation was very difficult: `map<string, vector<string>>`

---

## C++11 and beyond [Bob's slide]

_nullablepointer.requirements_

_allocator.requirements_

_pointer.traits_

_allocator.traits_

_allocator.adaptor_

_container.requirements.general_ [Allocator-aware containers] N4687

Containers use the `allocator_traits` template to get information about the allocator.

## Allocators

_The basic purpose of an allocator is to provide a source of memory for a given
type, and a place to return that memory to once it is no longer needed._

<sub>-Bjarne Stroustrup, The C++ Programming Language, 4th Edition</sub>

Allocators were part of Stepanov's original STL design. 

They provide a more granular way to manage memory than `new` and `delete`.

Allocators separate allocation and construction, and deallocation and destruction.

---

## Why write an allocator?

* Performance
  * Stack allocation
  * Container-specific memory pools
  * Thread-local allocation (lock-free)
  * Memory pools
  * Arena allocation

* Debugging / Instrumentation / Testing

* Relocatable data

* Shared memory
  
<sub>Thanks to Bob Steagall (cppcon 2017) for collating the above list.</sub>

---

## Allocator rebinding

TODO

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

## Allocator propagation [Bob's slide]

Lateral: Copy/move construction, copy/move assignment, swap.
controlled by allocator propagation traits

Deep: Nesting the allocator of the outermost container in a container heirarchy.
scoped_allocator_adaptor helps with this

---

## Allocator traits [Bob's slide]

Gets information about an allocator and provides it to a container.

Provides typedefs.

Provides construct and destroy functions if not speficied by the allocator.

Assumes propagation traits are false unless overridden.

Assumes that allocators compare equal if empty.

---

## Minimal allocator

```cpp
template <typename T>
struct minimal_allocator {
    using value_type = T;

    minimal_allocator(PARAMS);
    
    template<typename U> minimal_allocator(const minimal_allocator<U>&);

    T* allocate(std::size_t);

    void deallocate(T*, std::size_t);

    friend bool operator==(const minimal_allocator&, const minimal_allocator&);
};

```

---

## PMR allocators

Runtime polymorphism using a `pmr::memory_resource` base class.

PMR Allocators stick to the container, they do not propagate.

`pmr::polymorphic_allocator` wraps a `pmr::memory_resource` and provides the allocator interface.

The memory resource must outlive the allocators that use it.

---

## swap

Standard requires container iterators to be valid after a swap => UB in swap.

---

## `select_on_container_copy_construction`

---

## Scoped allocators

Container passes its allocator to the constructor of the elements that it constructs.

<sub>Thanks to Pablo Halpern and Alisdair Meredith for their excellent cppcon2019 talk</sub>

Adding `using allocator_type = Allocator;` to a class makes it allocator-aware and allows it to support scoped allocators.

---

## Allocator-extended constructors

---

## Allocators and C++ vocabulary types

---

## Acknowledgements

Thanks to:

* Nina Ranns for fielding our regular questions on allocators.

* Bob Steagall for his excellent cppcon 2017 talk on allocators.

* Joshua Berne for pair-debugging our early implementation of `polymorphic`.

* Neelofer Banglawala for her help with the slides.

* Assorted members of the C++ community for their ongoing work on allocators.

We've been the grateful recipients of a great deal of wisdom. Mistakes are our own.
