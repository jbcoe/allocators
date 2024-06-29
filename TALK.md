---
marp: true
theme: default
paginate: true
size: 16:9
---

# Allocators for C++ Vocabulary Types

_Jonathan B. Coe & Antony Peacock_

---

## Outline

1. Adding new types to the C++ standard.
1. A simple new type: `dyn_optional`.
1. Implementing `dyn_optional`
1. Introducing allocators.
1. A Lego analogy for allocators.
1. Adding allocator support to `dyn_optional`.

---

## Adding `indirect` and `polymorphic` to the C++ standard

We've been working on adding `indirect` and `polymorphic` to the C++ standard.

These two class templates are designed to be used for member data in composite
types.

* An instance of `indirect<T>` owns an object of class `T`.

* An instance of `polymorphic<T>` owns an object of class `T` or a class derived
  from `T`.

We've added allocator support to `indirect` and `polymorphic`.

Work progresses at https://github.com/jbcoe/value_types

---

## A simple new type: `dyn_optional`

We'll work on a new type, `dyn_optional`, for our examples.

`dyn_optional`, like `std::optional`, is a type that can hold a value or be
empty.

When `dyn_optional` is non-empty, the value is stored in dynamic memory.

There are not a host of good reasons to use `dyn_optional` but it should be
simple enough for our examples.

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

We want to use allocators to allocate and deallocate memory as they offer more control and flexibility.

Allocators allow us to customize object creation and destruction, and memory allocation and deallocation.

---

## Allocators

_The basic purpose of an allocator is to provide a source of memory for a given
type, and a place to return that memory to once it is no longer needed._

###### Bjarne Stroustrup, The C++ Programming Language, 4th Edition

They provide a more granular way to manage memory than `new` and `delete`.

Allocators separate allocation and construction, and deallocation and destruction.

---

## Why would we want to use an allocator?

TODO

–––

## A brief history of allocators

TODO

---

## Allocators: The Lego Analogy

While working on `indirect` and `polymorphic`, we came up with an analogy that made us re-think our code.

* Imagine that you're building a Lego model.

* The allocator is the box of Lego bricks that you use to build the model.

* Bricks (memory) are taken from the box and used to build (construct) the model.

* When the model is taken apart, the bricks are returned to the box.

---

## Allocators: The Lego Analogy II: A common pile of bricks

With the default allocator, bricks come from a common pile (heap).

![mixed-lego](images/multi-lego.jpeg)

---

## Allocators: The Lego Analogy III: Sorted bricks

With a custom allocator, bricks come from a specific box.

![sorted-lego](images/sorted-lego.jpeg)

---

## Allocators: The Lego Analogy IV: Scoped allocators

When we add more complexity to the model, we need more bricks.

We can use a different box for the new bricks or we can use the same box as the original model.

This maps rather neatly onto the idea of scoped allocators where a heirarchy of containers use the same allocator as the outermost container.

---

## Allocators: The Lego Analogy V: Allocator propagation

When we copy a model, we can use the same box of bricks or we can use a
different box.

Perhaps you're copying your friend's red house and need to use your supply of red bricks.

Whatever happens, we need to be sure that when the models are taken apart, the
bricks are returned to the box they originally came from.

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

## Allocator traits

TODO

---

## Making `dyn_optional` allocator-aware

TODO

---

## Allocator-extended constructors

TODO

---

## Allocator propagation

TODO

---

## `swap` in the presence of allocators

TODO

---

## TODO(REMOVE): C++03 allocators [Bob's slide]

Pointers were always `T*`.

Implementations assumed that allocators were stateless as instances always compared equal and could be considered interchangeable.

Shared memory data structures could not be readily implemented with standard library containers.

Scoped allocation was very difficult: `map<string, vector<string>>`

---

## TODO(REMOVE): C++11 and beyond [Bob's slide]

_nullablepointer.requirements_

_allocator.requirements_

_pointer.traits_

_allocator.traits_

_allocator.adaptor_

_container.requirements.general_ [Allocator-aware containers] N4687

Containers use the `allocator_traits` template to get information about the allocator.


## TODO(REMOVE): Why write an allocator?

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

## TODO(REMOVE): Allocator propagation [Bob's slide]

Lateral: Copy/move construction, copy/move assignment, swap.
controlled by allocator propagation traits

Deep: Nesting the allocator of the outermost container in a container heirarchy.
scoped_allocator_adaptor helps with this

---

## TODO(REMOVE): Allocator traits [Bob's slide]

Gets information about an allocator and provides it to a container.

Provides typedefs.

Provides construct and destroy functions if not speficied by the allocator.

Assumes propagation traits are false unless overridden.

Assumes that allocators compare equal if empty.

---

## TODO(REMOVE): Minimal allocator

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

## TODO(REMOVE): PMR allocators

Runtime polymorphism using a `pmr::memory_resource` base class.

PMR Allocators stick to the container, they do not propagate.

`pmr::polymorphic_allocator` wraps a `pmr::memory_resource` and provides the allocator interface.

The memory resource must outlive the allocators that use it.

---

## TODO(REMOVE): swap

Standard requires container iterators to be valid after a swap => UB in swap.

---

## TODO(REMOVE): Scoped allocators

Container passes its allocator to the constructor of the elements that it constructs.

<sub>Thanks to Pablo Halpern and Alisdair Meredith for their excellent cppcon2019 talk</sub>

Adding `using allocator_type = Allocator;` to a class makes it allocator-aware and allows it to support scoped allocators.

---

## Allocators and C++ vocabulary types

TODO

---

## Acknowledgements

Thanks to:

* Nina Ranns for fielding our regular questions on allocators.

* Bob Steagall for his excellent cppcon 2017 talk on allocators.

* Joshua Berne for pair-debugging our early implementation of `polymorphic`.

* Neelofer Banglawala for her help with the slides.

* Assorted members of the C++ community for their ongoing work on allocators.

We've been the grateful recipients of a great deal of wisdom. Mistakes are our own.
