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

#include <utility>

namespace xyz {
template <class T>
class dyn_optional {
 private:
  T* ptr;

 public:
    // Constructors
    dyn_optional() : ptr(nullptr) {}
    
    dyn_optional(const T& t) : ptr(new T(t)) {}

    // Copy and Move Constructor
    dyn_optional(const dyn_optional& other) : ptr(other.ptr ? new T(*other.ptr) : nullptr) {}
    
    dyn_optional(dyn_optional&& other) : ptr(other.ptr) { other.ptr = nullptr; }

    // Copy and Move Assignment
    dyn_optional& operator=(const dyn_optional& other) {
        dyn_optional tmp(other);
        tmp.swap(*this);
        return *this;
    }

    dyn_optional& operator=(dyn_optional&& other) {
        other.swap(*this);
        return *this;
    }

    // Observers
    operator bool() const { return ptr != nullptr; }

    // Const accessors
    const T& operator*() const { return *ptr; }
    const T* operator->() const { return ptr; }

    // Non-const accessors
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }

    // Modifiers
    void swap(dyn_optional& other) { std::swap(ptr, other.ptr); }
    void reset() { delete ptr; ptr = nullptr; }

    // Destructor
    ~dyn_optional() { delete ptr; }


};

}  // namespace xyz
