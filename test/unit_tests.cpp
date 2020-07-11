#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "heap_array.hpp"

TEST_CASE("It is possible to create heap_array by providing size",
          "[construction][uninitialized]") {
  const vlrx::heap_array<int> test_array(5);
  REQUIRE(test_array.size() == 5);
  REQUIRE(test_array.max_size() == 5);
}

TEST_CASE("It is possible to create heap_array from initializer list",
          "[construction][initializer list][pod]") {
  const vlrx::heap_array<int> test_array{1, 2, 3, 4, 5};
  REQUIRE(test_array.size() == 5);
  REQUIRE(test_array[0] == 1);
  REQUIRE(test_array[1] == 2);
  REQUIRE(test_array[2] == 3);
  REQUIRE(test_array[3] == 4);
  REQUIRE(test_array[4] == 5);
}

TEST_CASE("It is possible to modify data via subscript operator",
          "[modification][pod]") {
  vlrx::heap_array<int> test_array(5);
  REQUIRE(test_array.size() == 5);
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
    "Empty for non-empty containers returns false and for empty returns true",
    "[empty]") {
  const vlrx::heap_array<int> test_array{1, 2, 3};
  REQUIRE(test_array.empty() == false);
  const vlrx::heap_array<int> test_array1(0);
  REQUIRE(test_array1.empty() == true);
}

TEST_CASE("Data for non-empty containers returns pointer to the 0-th element",
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

TEST_CASE("It is possible to add or substract number from the iterator to get "
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