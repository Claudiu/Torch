cmake_minimum_required(VERSION 2.8)

project (test)

set(TORCH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." )
add_subdirectory(${TORCH_PATH} "${CMAKE_CURRENT_BINARY_DIR}/build-torch")

add_executable(test main.cpp)
include_directories(${TORCH_PATH}/include)
target_link_libraries(test torch)

