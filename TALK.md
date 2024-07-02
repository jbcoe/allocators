---
marp: true
theme: default
paginate: true
size: 16:9
---

<!-- paginate: false -->
![bg contain](images/title-slide.png)

---

<!-- paginate: true -->

## Outline

1. Adding new types to the C++ standard.
1. A simple new type: `dyn_optional`.
1. Implementing `dyn_optional`.
1. Introducing allocators.
1. A Lego analogy for allocators.
1. Adding allocator support to `dyn_optional`.

---

## Adding `indirect` and `polymorphic` to the C++ standard

We have been working on adding `indirect` and `polymorphic` to the C++ standard.

These two class templates are designed to be used for member data in composite
types.

* An instance of `indirect<T>` owns an object of class `T`.

* An instance of `polymorphic<T>` owns an object of class `T` or a class derived
  from `T`.

We have added allocator support to `indirect` and `polymorphic`.

Work progresses at https://github.com/jbcoe/value_types.

---

## A simple new type: `dyn_optional`

We will work on a new type, `dyn_optional`, for our examples.

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
    if(this!=&other) {
        delete ptr;
        ptr = other.ptr ? new T(*other.ptr) : nullptr;
    }
    return *this;
}
```

```cpp
template <typename T>
dyn_optional<T>& dyn_optional<T>::operator=(dyn_optional<T>&& other) {
    if(this!=&other) {
        delete ptr;
        ptr = std::exchange(other.ptr, nullptr);
    }
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

We used `new` to allocate memory for the value in `dyn_optional` and `delete` to deallocate it.

We want to use allocators to allocate and deallocate memory as they offer more control and flexibility.

Allocators allow us to customize object creation and destruction, and memory allocation and deallocation.

---

## Allocators

_The basic purpose of an allocator is to provide a source of memory for a given
type, and a place to return that memory to once it is no longer needed._

<small>Bjarne Stroustrup, The C++ Programming Language, 4th Edition</small>

Allocators provide a more granular way to manage memory than `new` and `delete`.

Allocators separate allocation and construction, and deallocation and destruction.

---

## Why would we want to use an allocator?

TODO

---

## The Lego Analogy

![bg right](images/Ant1.jpeg)

While working on `indirect` and `polymorphic`, we came up with an analogy that made us re-think our code.

Imagine that we are building a Lego model.

The allocator is the box of Lego bricks that you use to build the model.

---

## The Lego Analogy II: A common pile of bricks

With the default allocator, bricks come from a common pile (heap).

Bricks come out of the pile to build a model and go back into the pile when a model is taken apart.

![bg left](images/multi-lego.jpeg)

---

## The Lego Analogy III: Sorted bricks

With a custom allocator, bricks come from a specific box.

Red bricks come out of the red box and go back into the red box.

![bg right](images/sorted-lego.jpeg)

---

## The Lego Analogy IV: Scoped allocators

When we add more complexity to the model, for instance another turret on our castle, we need more bricks.

We can use a different box for the new bricks or we can use the same box as the original model.

With scoped allocators, a heirarchy of containers uses the same allocator as the outermost container.

![bg left](images/lego-castle.png)

---

## The Lego Analogy V: Allocator propagation

When we copy a model, we can use the same box of bricks or we can use a
different box.

Perhaps you are copying your friend's red house and need to use your supply of red bricks.

We must sure that when the models are taken apart, the bricks are returned to the correct box.

![bg right](images/two-houses.png)

---

## A brief history of allocators

Allocators were added to C++ as part of the STL to allow custom memory
management.

```cpp
std::vector<T, A=std::allocator<T>>
```

Before C++11, an allocator was a lightweight handle to a set of member functions
that affected how an object was constructed and destroyed, and how memory was
allocated and deallocated from some global resource.

C++11 introduced a more sophisticated model of allocators where allocators could
contain state and were interacted with through allocator traits.

---

## Adding an allocator to dyn_optional

We make an allocator part of `dyn_optional` and use the allocator to allocate and deallocate memory.

A user could then specify the allocator they want to use when they create a `dyn_optional`.

We add the allocator as a second template parameter to `dyn_optional`. By default we use `std::allocator` so that if a
user does not specify an allocator, we use the standard allocator just like other standard library types.

```cpp
template <typename T, typename Allocator = std::allocator<T>>
class dyn_optional;
```

---

## Allocator traits

Our class will interact with an allocator using allocator traits.

```cpp
Allocator allocator; // An instance of the allocator
using allocator_traits = std::allocator_traits<Allocator>;

// Allocate memory and construct an object.
auto memory = allocator_traits::allocate(allocator, 1);
allocator_traits::construct(allocator, memory, std::forward<Us>(us)...);

// Destroy an object and deallocate memory.
allocator_traits::destroy(allocator, memory);
allocator_traits::deallocate(allocator, memory, 1);
```

See https://en.cppreference.com/w/cpp/memory/allocator_traits for more information.

---

## Making `dyn_optional` allocator-aware

For scoped allocator support, a container needs to know if the objects it constructs need to be constructed with an allocator.

Types can advertise their allocator support by adding using declaration to their class definition:

```cpp
using allocator_type = Allocator;
```

Note that this is not a convenience but a requirement for scoped allocator support.

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

## Class definition with allocators

Add an allocator template argument to `dyn_optional`.

Add allocator-extended constructors.

```cpp
template <typename T, typename Allocator = std::allocator<T>>
class dyn_optional {
private:
    T* ptr;
    [[no_unique_address]] Allocator allocator;

public:
    // Constructors
    dyn_optional() noexcept;
    template <typename ...Us> dyn_optional(Us&& ...us);

    // Allocator-extended constructors
    dyn_optional(std::allocator_arg_t, Allocator const& a) noexcept;
    template <typename ...Us> dyn_optional(std::allocator_arg_t, Allocator const& a, Us&& ...us);
```

`[no_unique_address]` is a C++20 attribute that ensures that our object does not
increase in size when the allocator is stateless.

---

Add allocator-extended constructors.

```cpp
    // Copy and move constructors
    dyn_optional(const dyn_optional& other);
    dyn_optional(dyn_optional&& other);

    // Allocator-extended copy and move constructors
    dyn_optional(std::allocator_arg_t, Allocator const& a, const dyn_optional& other);
    dyn_optional(std::allocator_arg_t, Allocator const& a, dyn_optional&& other);

    // Assignment operators
    dyn_optional& operator=(const dyn_optional& other);
    dyn_optional& operator=(dyn_optional&& other);
```

---

No change to remaining member function interfaces.

```cpp
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

## `dyn_optional` Allocator-construction helper

```cpp
  using allocator_traits = std::allocator_traits<Allocator>;
  using pointer = typename allocator_traits::pointer;

  // ...

  template <typename... Us>
  static pointer construct(A alloc, Us&&... us) {
    pointer mem = allocator_traits::allocate(alloc, 1);
    try {
      allocator_traits::construct(alloc, std::to_address(mem),
                                  std::forward<Us>(us)...);
      return mem;
    } catch (...) {
      allocator_traits::deallocate(alloc, mem, 1);
      throw;
    }
  }
```

---

## `dyn_optional` Allocator-destruction helper

```cpp
  using allocator_traits = std::allocator_traits<Allocator>;
  using pointer = typename allocator_traits::pointer;

  // ...

  constexpr static void destroy(A alloc, pointer p) {
    allocator_traits::destroy(alloc, std::to_address(p));
    allocator_traits::deallocate(alloc, p, 1);
  }
```

---

## `dyn_optional<T,A>` Constructors with allocators

Use the allocator-construction helper in constructors

```cpp
dyn_optional() noexcept
    : allocator(), ptr(construct(allocator)) {}

template <typename ...Us>
dyn_optional(Us&& ...us) noexcept
    : allocator(), ptr(construct(allocator, std::forward<Us>(us)...)) {}
```

---

## `dyn_optional<T,A>` Allocator-extended constructors

Use the allocator-construction helper in allocator-extended constructors

```cpp
dyn_optional(std::allocator_arg_t, const A& alloc) noexcept
    : allocator(alloc), ptr(construct(allocator)) {}

template <typename ...Us>
dyn_optional(std::allocator_arg_t, const A& alloc, Us&& ...us) noexcept
    : allocator(alloc), ptr(construct(allocator, std::forward<Us>(us)...)) {}
```

---

## `dyn_optional<T,A>` Copy and move constructors

Use the allocator-construction helper in copy and move constructors

Use `select_on_container_copy_construction` to copy (or not) the allocator.

```cpp
```

---

## `dyn_optional<T,A>` Allocator-extended copy and move constructors

Use the allocator-construction helper in allocator-extended copy and move constructors

```cpp
```

---

## `dyn_optional<T,A>` Assignment operators

```cpp
```

---

## `dyn_optional<T,A>` Swap

```cpp
```

---

## `dyn_optional<T,A>` Destructor

```cpp
```

---

## Slide-ware (do not use example code)

We have knowingly omitted:

* `requires`
* `static_assert`
* `noexcept`
* `constexpr`
* `// Any sort of helpful comments`
* `[[nodiscard]]`
* Use of `pointer_traits` for fancy-pointer support

Code from a reference implementation for our proposal

"Vocabulary Types for Composite Class Design" https://wg21.link/P3019

is more complete and includes these features.

https://github.com/jbcoe/value_types

---

## Acknowledgements

Thanks to:

* My co-author Antony Peacock.

* Nina Ranns for fielding our regular questions on allocators.

* Joshua Berne for pair-debugging our early implementation of `polymorphic`.

* Neelofer Banglawala for her help with the slides and papers.

* Bob Steagall for his excellent cppcon 2017 talk on allocators.

* Assorted members of the C++ community for their ongoing work on allocators.

We have been the grateful recipients of a great deal of wisdom. Mistakes are our own.
