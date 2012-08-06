cmake_minimum_required(VERSION 2.8)

project (Torch)

set(TORCH_HEADERS
    include/torch/HTTP/application.hpp
    include/torch/http.hpp

    include/torch/Logs/logs.hpp
    include/torch/logs.hpp
    )

set(TORCH_SOURCES
    src/modules/Logs/logs.cpp
    )

add_library(torch ${TORCH_SOURCES} ${TORCH_HEADERS})
include_directories(include)
