cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(andromeda-engine LANGUAGES CXX)

include(cmake/requirements.cmake)

add_subdirectory(andromeda)
add_subdirectory(example)