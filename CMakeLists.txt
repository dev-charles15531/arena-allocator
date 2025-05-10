cmake_minimum_required(VERSION 3.15)
project(arena_alloc LANGUAGES C)

add_library(arena_alloc STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/arena_alloc.c)

target_include_directories(arena_alloc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(arena_alloc PROPERTIES POSITION_INDEPENDENT_CODE ON)
