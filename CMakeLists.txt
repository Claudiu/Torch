cmake_minimum_required(VERSION 2.8)

project (Torch)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g")

set(TORCH_HEADERS
    include/torch/HTTP/headers.hpp
    include/torch/HTTP/application.hpp
    include/torch/http.hpp

    include/torch/Logs/logs.hpp
    include/torch/logs.hpp

    include/torch/Sockets/socket.hpp
    include/torch/sockets.hpp
    )

set(TORCH_SOURCES
    src/modules/Logs/logs.cpp
    src/modules/HTTP/application.cpp
    src/modules/Sockets/socket.cpp
    )

add_library(torch ${TORCH_SOURCES} ${TORCH_HEADERS})
include_directories(include)