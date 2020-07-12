#pragma once

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace vlrx {

template <typename T, typename SizeType = std::uint64_t>
class heap_array final {
  template <bool is_const = false> class random_access_iterator final {
  public:
    random_access_iterator() noexcept : ptr_{} {};

    using difference_type = std::int64_t;
    using value_type = T;
    using pointer =
        typename std::conditional_t<is_const, const value_type *, value_type *>;
    using reference =
        typename std::conditional_t<is_const, const value_type &, value_type &>;
    using const_reference = const value_type &;
    using iterator_category = std::random_access_iterator_tag;

    reference operator*() noexcept { return *ptr_; }

    const_reference operator*() const noexcept { return *ptr_; }

    reference operator[](const difference_type shift) noexcept {
      return *(ptr_ + shift);
    }

    const_reference operator[](const difference_type shift) const noexcept {
      return *(ptr_ + shift);
    }

    friend random_access_iterator &
    operator++(random_access_iterator &iter) noexcept {
      ++iter.ptr_;
      return iter;
    }

    friend random_access_iterator operator++(random_access_iterator &iter,
                                             int) noexcept {
      auto retval = iter;
      ++iter.ptr_;
      return retval;
    }

    friend random_access_iterator &
    operator--(random_access_iterator &iter) noexcept {
      --iter.ptr_;
      return iter;
    }

    friend random_access_iterator operator--(random_access_iterator &iter,
                                             int) noexcept {
      auto retval = iter;
      --iter.ptr_;
      return retval;
    }

    friend random_access_iterator &operator+=(random_access_iterator &iter,
                                              const difference_type shift) {
      iter.ptr_ += shift;
      return iter;
    }

    friend random_access_iterator &operator-=(random_access_iterator &iter,
                                              const difference_type shift) {
      iter.ptr_ -= shift;
      return iter;
    }

    friend random_access_iterator operator+(const random_access_iterator &iter,
                                            const difference_type shift) {
      return random_access_iterator{iter.ptr_ + shift};
    }

    friend random_access_iterator
    operator+(const difference_type shift, const random_access_iterator &iter) {
      return random_access_iterator{iter.ptr_ + shift};
    }

    friend random_access_iterator operator-(const random_access_iterator &iter,
                                            const difference_type shift) {
      return random_access_iterator{iter.ptr_ - shift};
    }

    friend difference_type operator-(const random_access_iterator &lhs,
                                     const random_access_iterator &rhs) {
      return static_cast<difference_type>(lhs.ptr_ - rhs.ptr_);
    }

    friend random_access_iterator
    operator-(const difference_type shift, const random_access_iterator &iter) {
      return random_access_iterator{iter.ptr_ - shift};
    }

    friend bool operator==(const random_access_iterator &lhs,
                           const random_access_iterator &rhs) {
      return lhs.ptr_ == rhs.ptr_;
    }

    friend bool operator!=(const random_access_iterator &lhs,
                           const random_access_iterator &rhs) {
      return !(lhs == rhs);
    }

    friend bool operator<(const random_access_iterator &lhs,
                          const random_access_iterator &rhs) {
      return lhs.ptr_ < rhs.ptr_;
    }

    friend bool operator>(const random_access_iterator &lhs,
                          const random_access_iterator &rhs) {
      return rhs.ptr_ < lhs.ptr_;
    }

    friend bool operator<=(const random_access_iterator &lhs,
                           const random_access_iterator &rhs) {
      return !(lhs.ptr_ > rhs.ptr_);
    }

    friend bool operator>=(const random_access_iterator &lhs,
                           const random_access_iterator &rhs) {
      return !(lhs.ptr_ < rhs.ptr_);
    }

  private:
    friend heap_array<value_type>;

    explicit random_access_iterator(const pointer ptr) noexcept : ptr_{ptr} {}

    pointer ptr_;
  };

public:
  using value_type = T;
  using size_type = SizeType;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const T *;
  using iterator = random_access_iterator<false>;
  using const_iterator = random_access_iterator<true>;
  using reverse_iterator = random_access_iterator<false>; // TODO implement this
  using const_reverse_iterator =
      random_access_iterator<true>; // TODO implement this

  explicit heap_array(const size_type size) : storage_{}, size_{size} {
    if (size_ > 0) {
      storage_ = new storage_type[size_];
    }
  }

  heap_array(std::initializer_list<value_type> init)
      : storage_{}, size_{init.size()} {
    assert(init.size() <= std::numeric_limits<size_type>::max());
    if (size_ > 0) {
      storage_ = new storage_type[init.size()];
      size_type idx{0};
      for (auto &val : init) {
        new (storage_ + idx) value_type(val); // TODO is this correct?
        ++idx;
      }
    }
  }

  template <typename OtherSizeType>
  heap_array(const heap_array<T, OtherSizeType> &other) {
    assert(other.size_ <= std::numeric_limits<size_type>::max());
    // TODO implement this
  }

  const_reference at(const size_type pos) const {
    if (pos >= size_) {
      throw std::out_of_range("Tryin to access element which is out of range");
    }
    return *to_value_type_pointer(storage_ + pos);
  }

  reference at(const size_type pos) {
    if (pos >= size_) {
      throw std::out_of_range("Tryin to access element which is out of range");
    }
    return *to_value_type_pointer(storage_ + pos);
  }

  const_reference operator[](const size_type pos) const noexcept {
    assert(pos < size_);
    return *to_value_type_pointer(storage_ + pos);
  }

  reference operator[](const size_type pos) noexcept {
    assert(pos < size_);
    return *to_value_type_pointer(storage_ + pos);
  }

  reference front() noexcept { return *to_value_type_pointer(storage_); }

  const_reference front() const noexcept {
    return *to_value_type_pointer(storage_);
  }

  reference back() noexcept {
    return *to_value_type_pointer(storage_ + size_ - 1);
  }

  const_reference back() const noexcept {
    return *to_value_type_pointer(storage_ + size_ - 1);
  }

  pointer data() noexcept { return to_value_type_pointer(storage_); }

  const_pointer data() const noexcept {
    return to_value_type_pointer(storage_);
  }

  iterator begin() noexcept {
    return iterator{to_value_type_pointer(storage_)};
  }

  const_iterator begin() const noexcept {
    return const_iterator{to_value_type_pointer(storage_)};
  }

  const_iterator cbegin() const noexcept {
    return const_iterator{to_value_type_pointer(storage_)};
  }

  iterator end() noexcept {
    return iterator{to_value_type_pointer(storage_ + size_)};
  }

  const_iterator end() const noexcept {
    return const_iterator{to_value_type_pointer(storage_ + size_)};
  }

  const_iterator cend() const noexcept {
    return const_iterator{to_value_type_pointer(storage_ + size_)};
  }

  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  size_type max_size() const noexcept { return size_; }

  ~heap_array() { reset_storage(); }

private:
  using storage_type = typename std::aligned_storage<sizeof(value_type),
                                                     alignof(value_type)>::type;
  storage_type *storage_;
  size_type size_;

  pointer to_value_type_pointer(storage_type *storage_pointer) {
    return std::launder(reinterpret_cast<value_type *>(storage_pointer));
  }

  const_pointer to_value_type_pointer(storage_type *storage_pointer) const {
    return std::launder(reinterpret_cast<value_type *>(storage_pointer));
  }

  void reset_storage() noexcept {
    for (size_type i{}; i < size_; ++i) {
      to_value_type_pointer(storage_ + i)->~value_type();
    }
    delete[] storage_;
  }
};

} // namespace vlrx