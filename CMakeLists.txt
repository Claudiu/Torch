cmake_minimum_required(VERSION 2.8)

project (Torch)

set(TORCH_HEADERS
    include/torch/modules/HTTP/application.hpp
    include/torch/modules/http.hpp
    include/torch/modules/Logs/logs.hpp
    )

set(TORCH_SOURCES
    src/modules/Logs/logs.cpp
    )

add_library(torch ${TORCH_SOURCES} ${TORCH_HEADERS})
include_directories(include)
