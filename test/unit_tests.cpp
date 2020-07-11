#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "heap_array.hpp"

TEST_CASE("It is possible to create heap_array by providing size",
          "[construction][uninitialized]") {
  const vlrx::heap_array<int> test_array(5);
  REQUIRE(test_array.size() == 5);
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