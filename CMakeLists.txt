cmake_minimum_required(VERSION 3.17)
project(heap_array)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(heap_array
    INTERFACE
)

target_sources(heap_array
    INTERFACE
        include/heap_array.hpp
)

target_include_directories(heap_array
    INTERFACE
        include
)

target_compile_features(heap_array
    INTERFACE 
        cxx_std_17
)

target_compile_options(heap_array
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

add_subdirectory(test)