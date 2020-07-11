
add_executable(unit_tests)

add_test(unit_tests unit_tests)

target_compile_features(unit_tests PRIVATE cxx_std_17)

target_sources(unit_tests
    PRIVATE
        unit_tests.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/third_party
)