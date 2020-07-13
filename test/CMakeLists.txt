
add_executable(unit_tests)

add_test(unit_tests unit_tests)

target_sources(unit_tests
    PRIVATE
        unit_tests.cpp
)

target_compile_options(unit_tests
    PRIVATE
        -fsanitize=address
)

target_link_options(unit_tests
    PRIVATE
        -fsanitize=address
)

target_include_directories(unit_tests
    SYSTEM
        PRIVATE
            ${PROJECT_SOURCE_DIR}/third_party/catch2
)

target_link_libraries(unit_tests
    PRIVATE
        heap_array
)