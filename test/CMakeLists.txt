
add_executable(unit_tests)

add_test(unit_tests unit_tests)

target_sources(unit_tests
    PRIVATE
        unit_tests.cpp
)

target_include_directories(unit_tests
    SYSTEM
        PRIVATE
            ${PROJECT_SOURCE_DIR}/third_party
)

target_link_libraries(unit_tests
    PRIVATE
        heap_array
)