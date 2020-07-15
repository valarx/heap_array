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
  template <bool is_const = false>
  class [[nodiscard]] random_access_iterator final {
  public:
    random_access_iterator() noexcept : ptr_{} {};

    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer =
        typename std::conditional_t<is_const, const value_type *, value_type *>;
    using reference =
        typename std::conditional_t<is_const, const value_type &, value_type &>;
    using const_pointer = const value_type *;
    using const_reference = const value_type &;
    using iterator_category = std::random_access_iterator_tag;

    reference operator*() noexcept { return *ptr_; }

    const_reference operator*() const noexcept { return *ptr_; }

    pointer operator->() noexcept { return ptr_; }

    const_pointer operator->() const noexcept { return ptr_; }

    reference operator[](const difference_type shift) noexcept {
      return *(ptr_ + shift);
    }

    const_reference operator[](const difference_type shift) const noexcept {
      return *(ptr_ + shift);
    }

    friend random_access_iterator &operator++(
        random_access_iterator &iter) noexcept {
      ++iter.ptr_;
      return iter;
    }

    friend random_access_iterator operator++(random_access_iterator &iter,
                                             int) noexcept {
      auto retval = iter;
      ++iter.ptr_;
      return retval;
    }

    random_access_iterator &operator--() noexcept {
      --ptr_;
      return *this;
    }

    random_access_iterator operator--(int) noexcept {
      auto retval = *this;
      --(*this).ptr_;
      return retval;
    }

    random_access_iterator &operator+=(const difference_type shift) {
      (*this).ptr_ += shift;
      return *this;
    }

    random_access_iterator &operator-=(const difference_type shift) {
      (*this).ptr_ -= shift;
      return *this;
    }

    friend random_access_iterator operator+(const random_access_iterator &iter,
                                            const difference_type shift) {
      return random_access_iterator{iter.ptr_ + shift};
    }

    friend random_access_iterator operator+(
        const difference_type shift, const random_access_iterator &iter) {
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

    friend random_access_iterator operator-(
        const difference_type shift, const random_access_iterator &iter) {
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

  heap_array(std::initializer_list<value_type> init) : storage_{}, size_{} {
    assert(init.size() <= std::numeric_limits<size_type>::max());
    auto buffer = allocate_buffer(static_cast<size_type>(init.size()));
    try {
      fill_buffer(init.begin(), buffer, static_cast<size_type>(init.size()));
    } catch (...) {
      delete[] buffer;
      throw;
    }
    set_up_storage(buffer, static_cast<size_type>(init.size()));
  }

  heap_array(const heap_array &other) : storage_{}, size_{} {
    auto buffer = allocate_buffer(other.size_);
    try {
      fill_buffer(other.begin(), buffer, other.size_);
    } catch (...) {
      delete[] buffer;
      throw;
    }
    set_up_storage(buffer, other.size_);
  }

  heap_array &operator=(const heap_array &other) {
    if (other.size_ != size_) {
      auto buffer = allocate_buffer(other.size_);
      try {
        fill_buffer(other.begin(), buffer, other.size_);
      } catch (...) {
        delete[] buffer;
        throw;
      }
      destroy_stored_objects();
      deallocate_storage();
      set_up_storage(buffer, other.size_);
    } else {
      [[maybe_unused]] auto res =
          std::copy(other.begin(), other.end(), begin());
    }
    return *this;
  }

  heap_array(heap_array &&other) : storage_{}, size_{} {
    size_ = static_cast<size_type>(other.size_);
    storage_ = other.storage_;
    other.size_ = 0;
    other.storage_ = nullptr;
  }

  heap_array<value_type, size_type> &operator=(heap_array &&other) {
    destroy_stored_objects();
    deallocate_storage();
    size_ = static_cast<size_type>(other.size_);
    storage_ = other.storage_;
    other.size_ = 0;
    other.storage_ = nullptr;
    return *this;
  }

  [[nodiscard]] const_reference at(const size_type pos) const {
    if (pos >= size_) {
      throw std::out_of_range("Trying to access element which is out of range");
    }
    return *to_value_type_pointer(storage_ + pos);
  }

      [[nodiscard]] reference at(const size_type pos) {
    if (pos >= size_) {
      throw std::out_of_range("Trying to access element which is out of range");
    }
    return *to_value_type_pointer(storage_ + pos);
  }

  [[nodiscard]] const_reference operator[](const size_type pos) const noexcept {
    assert(pos < size_);
    return *to_value_type_pointer(storage_ + pos);
  }

  [[nodiscard]] reference operator[](const size_type pos) noexcept {
    assert(pos < size_);
    return *to_value_type_pointer(storage_ + pos);
  }

  [[nodiscard]] reference front() noexcept {
    return *to_value_type_pointer(storage_);
  }

  [[nodiscard]] const_reference front() const noexcept {
    return *to_value_type_pointer(storage_);
  }

  [[nodiscard]] reference back() noexcept {
    return *to_value_type_pointer(storage_ + size_ - 1);
  }

  [[nodiscard]] const_reference back() const noexcept {
    return *to_value_type_pointer(storage_ + size_ - 1);
  }

  [[nodiscard]] pointer data() noexcept {
    return to_value_type_pointer(storage_);
  }

  [[nodiscard]] const_pointer data() const noexcept {
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

  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

  [[nodiscard]] size_type size() const noexcept { return size_; }

  [[nodiscard]] size_type max_size() const noexcept { return size_; }

  void swap(heap_array &other) {
    const auto temp_storage = storage_;
    const auto temp_size = size_;
    storage_ = other.storage_;
    size_ = other.size_;
    other.size_ = temp_size;
    other.storage_ = temp_storage;
  }

  ~heap_array() {
    if constexpr (std::is_trivially_destructible_v<value_type> == false) {
      destroy_stored_objects();
    }
    deallocate_storage();
  }

  template <typename VType, typename SType>
  friend void swap(heap_array<VType, SType> &lhs,
                   heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator==(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator!=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator<(const heap_array<VType, SType> &lhs,
                        const heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator>(const heap_array<VType, SType> &lhs,
                        const heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator<=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs);
  template <typename VType, typename SType>
  friend bool operator>=(const heap_array<VType, SType> &lhs,
                         const heap_array<VType, SType> &rhs);

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

  void destroy_stored_objects() noexcept {
    for (size_type i{}; i < size_; ++i) {
      to_value_type_pointer(storage_ + i)->~value_type();
    }
  }

  void deallocate_storage() noexcept {
    delete[] storage_;
    size_ = 0;
    storage_ = nullptr;
  }

  void set_up_storage(storage_type *buffer, const size_type size) noexcept {
    storage_ = buffer;
    size_ = size;
  }

  [[nodiscard]] storage_type *allocate_buffer(const size_type size) {
    storage_type *buffer{};
    if (size > 0) {
      buffer = new storage_type[size];
    }
    return buffer;
  }

  template <typename Input>
  void fill_buffer(Input &&iter, storage_type *&storage, const size_type size) {
    for (size_type idx{}; idx < size; ++idx) {
      new (storage + idx) value_type(iter[idx]);
    }
  }
};

template <typename VType, typename SType>
inline void swap(heap_array<VType, SType> &lhs, heap_array<VType, SType> &rhs) {
  lhs.swap(rhs);
}

template <typename VType, typename SType>
inline bool operator==(const heap_array<VType, SType> &lhs,
                       const heap_array<VType, SType> &rhs) {
  return lhs.size_ == rhs.size_ &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename VType, typename SType>
inline bool operator!=(const heap_array<VType, SType> &lhs,
                       const heap_array<VType, SType> &rhs) {
  return !(lhs == rhs);
}

template <typename VType, typename SType>
inline bool operator<(const heap_array<VType, SType> &lhs,
                      const heap_array<VType, SType> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename VType, typename SType>
inline bool operator>(const heap_array<VType, SType> &lhs,
                      const heap_array<VType, SType> &rhs) {
  return rhs < lhs;
}

template <typename VType, typename SType>
inline bool operator<=(const heap_array<VType, SType> &lhs,
                       const heap_array<VType, SType> &rhs) {
  return !(lhs > rhs);
}

template <typename VType, typename SType>
inline bool operator>=(const heap_array<VType, SType> &lhs,
                       const heap_array<VType, SType> &rhs) {
  return !(lhs < rhs);
}

} // namespace vlrx
