#pragma once

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

namespace vlrx {

template <typename T, typename SizeType = std::uint64_t>
class heap_array final {
  using storage_type =
      typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  storage_type *storage_;
  SizeType size_;

  T *to_value_type_pointer(storage_type *storage_pointer) {
    return std::launder(reinterpret_cast<value_type *>(storage_pointer));
  }

  const T *to_value_type_pointer(storage_type *storage_pointer) const {
    return std::launder(reinterpret_cast<value_type *>(storage_pointer));
  }

public:
  using value_type = T;
  using size_type = SizeType;

  explicit heap_array(const size_type size) : storage_{}, size_{size} {
    assert(size_ > 0);

    storage_ = new storage_type[size_];
  }

  heap_array(std::initializer_list<value_type> init)
      : storage_{}, size_{init.size()} {
    assert(init.size() <= std::numeric_limits<size_type>::max());

    storage_ = new storage_type[init.size()];
    size_type idx{0};
    for (auto &val : init) {
      new (storage_ + idx) value_type(val); // TODO is this correct?
      ++idx;
    }
  }

  template <typename U, typename OtherSizeType>
  heap_array(const heap_array<U, OtherSizeType> &other) {
    assert(other.size_ <= std::numeric_limits<size_type>::max());
    // TODO implement this
  }

  const value_type &operator[](const size_type pos) const {
    assert(pos < size_);
    // note: needs std::launder as of C++17
    return *to_value_type_pointer(&storage_[pos]);
  }

  value_type &operator[](const size_type pos) {
    assert(pos < size_);
    // note: needs std::launder as of C++17
    return *to_value_type_pointer(&storage_[pos]);
  }

  size_type size() const { return size_; }

  ~heap_array() {
    // TODO use std::launder
    for (size_type i{}; i < size_; ++i) {
      to_value_type_pointer(storage_ + i)->~value_type();
    }
    delete[] storage_;
  }
};

} // namespace vlrx