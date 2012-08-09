cmake_minimum_required(VERSION 2.8)

project (Torch)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g")

set(TORCH_HEADERS
    include/torch/Util/singleton.hpp
    include/torch/Util/exception.hpp
    include/torch/util.hpp
    
    include/torch/HTTP/headers.hpp
    include/torch/HTTP/application.hpp
    include/torch/HTTP/Response/response.hpp
    include/torch/HTTP/request.hpp
    include/torch/http.hpp

    include/torch/log.hpp

    include/torch/Sockets/socket.hpp
    include/torch/Sockets/select.hpp
    include/torch/sockets.hpp
    )

set(TORCH_SOURCES
    src/modules/Util/exception.cpp

    src/modules/log.cpp

    src/modules/HTTP/application.cpp
    src/modules/HTTP/Response/response.cpp
    src/modules/HTTP/request.cpp

    src/modules/Sockets/socket.cpp
    src/modules/Sockets/select.cpp
    )

add_library(torch ${TORCH_SOURCES} ${TORCH_HEADERS})
include_directories(include)
