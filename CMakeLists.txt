cmake_minimum_required(VERSION 3.10)

project(
    neba
    VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    neba
    src/main.cpp
)

target_include_directories(
    neba PRIVATE
    ${VENDOR_DIR}/include
)

target_link_directories(
    neba PRIVATE
    ${VENDOR_DIR}/lib
)

target_sources(
    neba PRIVATE
)

target_link_libraries(neba raylib)
