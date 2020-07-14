
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "heap_array.hpp"

#include <cstdint>

TEST_CASE("It is possible to create heap_array by providing size",
          "[construction][uninitialized]") {
  const vlrx::heap_array<int> test_array(5);
  REQUIRE(test_array.size() == 5);
  REQUIRE(test_array.max_size() == 5);
}

TEST_CASE("It is possible to create heap_array from initializer list",
          "[construction][initializer list][pod]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  REQUIRE(test_array.size() == 5);
  REQUIRE(test_array[0] == 1);
  REQUIRE(test_array[1] == 2);
  REQUIRE(test_array[2] == 3);
  REQUIRE(test_array[3] == 4);
  REQUIRE(test_array[4] == 5);
  test_array[4] = 3;
  REQUIRE(test_array[4] == 3);
}

TEST_CASE("Checked access throws on out of bounds access",
          "[checked access][out of bounds]") {
  const vlrx::heap_array<int> test_array{1, 2, 3};
  REQUIRE(test_array.at(2) == 3);
  REQUIRE_THROWS(test_array.at(3));
}

TEST_CASE("Front and back on non-empty containers return proper data",
          "[front][back]") {
  const vlrx::heap_array<int> test_array{1, 2, 3};
  REQUIRE(test_array.front() == 1);
  REQUIRE(test_array.back() == 3);
}

TEST_CASE(
    "Empty for non-empty containers returns false and for empty returns true ",
    "[empty]") {
  const vlrx::heap_array<int> test_array{1, 2, 3};
  REQUIRE(test_array.empty() == false);
  const vlrx::heap_array<int> test_array1(0);
  REQUIRE(test_array1.empty() == true);
}

TEST_CASE("Data for non-empty containers returns pointer to the 0-th element ",
          "[data]") {
  vlrx::heap_array<int> test_array{1, 2, 3};
  REQUIRE(test_array.data() != nullptr);
  test_array.data()[1] = 3;
  REQUIRE(test_array[1] == 3);
}

TEST_CASE("Begin and end return proper iterators, iterators are incrementable",
          "[iterators][begin][end][increment]") {
  vlrx::heap_array<int> test_array{1, 2, 3};
  auto iter = test_array.begin();
  REQUIRE(*iter == 1);
  ++iter;
  ++iter;
  ++iter;
  REQUIRE(iter == test_array.end());
}

TEST_CASE("It is possible to add or substract number from the iterator to get"
          "another iterator. It is possible to substract iterators",
          "[iterators][arithmetic]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  auto iter = test_array.begin();
  auto iter1 = iter + 2;
  REQUIRE(*iter1 == 3);
  iter1 = 2 + iter1;
  REQUIRE(*iter1 == 5);
  iter1 = iter1 - 1;
  REQUIRE(*iter1 == 4);
  iter1 += 1;
  REQUIRE(*iter1 == 5);
  iter1 -= 1;
  REQUIRE(*iter1 == 4);
  const auto iter2 = test_array.begin();
  REQUIRE(iter2[2] == 3);
  REQUIRE(test_array.end() - test_array.begin() == 5);
}

TEST_CASE("Comparison operators for iterator", "[iterator][comparison]") {
  {
    const vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
    REQUIRE(test_array.begin() != test_array.end());
    REQUIRE(test_array.begin() == test_array.begin());
    REQUIRE(test_array.begin() < test_array.end());
    REQUIRE(test_array.end() > test_array.begin());
    REQUIRE(test_array.begin() <= test_array.begin());
    REQUIRE(test_array.begin() >= test_array.begin());
    REQUIRE(test_array.begin() <= test_array.begin() + 1);
    REQUIRE(test_array.begin() + 1 >= test_array.begin());
  }
  {
    vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
    REQUIRE(test_array.begin() != test_array.end());
    REQUIRE(test_array.begin() == test_array.begin());
    REQUIRE(test_array.begin() < test_array.end());
    REQUIRE(test_array.end() > test_array.begin());
    REQUIRE(test_array.begin() <= test_array.begin());
    REQUIRE(test_array.begin() >= test_array.begin());
    REQUIRE(test_array.begin() <= test_array.begin() + 1);
    REQUIRE(test_array.begin() + 1 >= test_array.begin());
  }
}

TEST_CASE("Copy/move constructors/assignment operators work", "[copy][move]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  vlrx::heap_array<int> moved_to{std::move(test_array)};
  REQUIRE(moved_to[1] == 2);
  vlrx::heap_array<int> copy{moved_to};
  REQUIRE(moved_to[1] == 2);
  REQUIRE(copy[1] == 2);
}

TEST_CASE("Reverse iterators are present", "[reverse][iterators]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  REQUIRE(*test_array.rbegin() == 5);
  const vlrx::heap_array<int> const_test_array{1, 2, 3, 4, 5};
  REQUIRE(*const_test_array.rbegin() == 5);
  REQUIRE(*--test_array.rend() == 1);
  REQUIRE(*--const_test_array.rend() == 1);
}

TEST_CASE("Fill fills", "[fill]") {
  vlrx::heap_array<int> test_array(5);
  test_array.fill(1);
  for (const auto &val : test_array) {
    REQUIRE(val == 1);
  }
}

TEST_CASE("Swap swaps", "[swap]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  vlrx::heap_array<int> test_array1{6, 7, 8, 9, 10};
  test_array.swap(test_array1);
  REQUIRE(test_array == vlrx::heap_array<int>{6, 7, 8, 9, 10});
  REQUIRE(test_array1 == vlrx::heap_array<int>{1, 2, 3, 4, 5});
  swap(test_array1, test_array);
  REQUIRE(test_array1 == vlrx::heap_array<int>{6, 7, 8, 9, 10});
  REQUIRE(test_array == vlrx::heap_array<int>{1, 2, 3, 4, 5});
}

TEST_CASE("Comparison operator", "[heap_array][comparison]") {
  REQUIRE(vlrx::heap_array<int>{1, 2, 3, 4, 5} <
          vlrx::heap_array<int>{6, 7, 8, 9, 10});
  REQUIRE(vlrx::heap_array<int>{6, 7, 8, 9, 10} >
          vlrx::heap_array<int>{1, 2, 3, 4, 5});
  REQUIRE(vlrx::heap_array<int>{6, 7, 8, 9, 10} >=
          vlrx::heap_array<int>{1, 2, 3, 4, 5});
  REQUIRE(vlrx::heap_array<int>{6, 7, 8, 9, 10} >=
          vlrx::heap_array<int>{6, 7, 8, 9, 10});
  REQUIRE(vlrx::heap_array<int>{1, 2, 3, 4, 5} <=
          vlrx::heap_array<int>{6, 7, 8, 9, 10});
  REQUIRE(vlrx::heap_array<int>{1, 2, 3, 4, 5} <=
          vlrx::heap_array<int>{1, 2, 3, 4, 5});
  REQUIRE(vlrx::heap_array<int>{1, 2, 3, 4, 5} ==
          vlrx::heap_array<int>{1, 2, 3, 4, 5});
  REQUIRE(vlrx::heap_array<int>{1, 2, 3, 4, 5} !=
          vlrx::heap_array<int>{6, 7, 8, 9, 10});
}

TEST_CASE("Range based for loop", "[range for]") {
  vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  std::uint64_t idx{};
  for (const auto &val : test_array) {
    REQUIRE(val == test_array[idx]);
    ++idx;
  }
}

struct mock_struct {
  explicit mock_struct(std::uint16_t &counter) : counter_{counter} {}
  ~mock_struct() { ++counter_; }
  std::uint16_t &counter_;
};

TEST_CASE("Destructors are properly called, there is -> operator for iterator",
          "[destructors][arrow operator][iterator]") {
  std::uint16_t counter{};
  {
    vlrx::heap_array<mock_struct> test_array{
        mock_struct{counter}, mock_struct{counter}, mock_struct{counter}};
    REQUIRE(test_array.begin()->counter_ == (*test_array.begin()).counter_);
  }
  REQUIRE(counter == 6); // struct destructor is called twice, once for
                         // initializer list, once for heap vector
}
