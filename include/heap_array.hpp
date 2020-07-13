#pragma once

#include <algorithm>
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
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  explicit heap_array(const size_type size) : storage_{}, size_{size} {
    if (size_ > 0) {
      storage_ = new storage_type[size_];
    }
  }

  heap_array(std::initializer_list<value_type> init) : storage_{}, size_{} {
    assert(init.size() <= std::numeric_limits<size_type>::max());
    initialize_from([&init](const size_type idx) { return init.begin()[idx]; },
                    init.size());
  }

  heap_array(const heap_array &other) : storage_{}, size_{} {
    initialize_from([&other](const size_type idx) { return other[idx]; },
                    other.size_);
  }

  heap_array &operator=(const heap_array &other) {
    initialize_from([&other](const size_type idx) { return other[idx]; },
                    other.size_);
    return *this;
  }

  heap_array(heap_array &&other) : storage_{}, size_{} {
    size_ = static_cast<size_type>(other.size_);
    storage_ = other.storage_;
    other.size_ = 0;
    other.storage_ = nullptr;
  }

  heap_array<value_type, size_type> &operator=(heap_array &&other) {
    size_ = static_cast<size_type>(other.size_);
    storage_ = other.storage_;
    other.size_ = 0;
    other.storage_ = nullptr;
    return *this;
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

  reverse_iterator rbegin() noexcept {
    return reverse_iterator{iterator{to_value_type_pointer(storage_ + size_)}};
  }

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{
        const_iterator{to_value_type_pointer(storage_ + size_)}};
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{
        const_iterator{to_value_type_pointer(storage_ + size_)}};
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

  reverse_iterator rend() noexcept {
    return reverse_iterator{iterator{to_value_type_pointer(storage_)}};
  }

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{
        const_iterator{to_value_type_pointer(storage_)}};
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{
        const_iterator{to_value_type_pointer(storage_)}};
  }

  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  size_type max_size() const noexcept { return size_; }

  void fill(const_reference val) {
    initialize_from([&val](const size_type) { return val; }, size_);
  }

  void swap(heap_array &other) {
    const auto temp_storage = storage_;
    const auto temp_size = size_;
    storage_ = other.storage_;
    size_ = other.size_;
    other.size_ = temp_size;
    other.storage_ = temp_storage;
  }

  template <typename VType, typename SType>
  friend void swap(heap_array<VType, SType> &lhs,
                   heap_array<VType, SType> &rhs) {
    lhs.swap(rhs);
  }

  ~heap_array() { reset_storage(); }

  template <typename VType, typename SType>
  friend bool operator==(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs) {
    return lhs.size_ == rhs.size_ && [&lhs, &rhs]() {
      SType idx{};
      for (const auto &val : lhs) {
        if (val != rhs[idx]) {
          return false;
        }
        ++idx;
      }
      return true;
    }();
  }

  template <typename VType, typename SType>
  friend bool operator!=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs) {
    return !(lhs == rhs);
  }

  template <typename VType, typename SType>
  friend bool operator<(const heap_array<VType, SType> &lhs,
                        const heap_array<VType, SType> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
  }

  template <typename VType, typename SType>
  friend bool operator>(const heap_array<VType, SType> &lhs,
                        const heap_array<VType, SType> &rhs) {
    return rhs < lhs;
  }

  template <typename VType, typename SType>
  friend bool operator<=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs) {
    return !(lhs > rhs);
  }

  template <typename VType, typename SType>
  friend bool operator>=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs) {
    return !(lhs < rhs);
  }

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
    size_ = 0;
    storage_ = nullptr;
  }

  template <typename ValueGenerator>
  void initialize_from(ValueGenerator &&generator, const size_type size) {
    storage_type *storage{};
    if (size > 0) {
      storage = new storage_type[size];
      for (size_type idx{}; idx < size; ++idx) {
        new (storage + idx) value_type(generator(idx));
      }
    }
    reset_storage();
    size_ = size;
    storage_ = storage;
  }
};

} // namespace vlrx
