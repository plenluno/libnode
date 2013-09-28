# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(jsonrpc)

# build libraries --------------------------------------------------------------

add_library(jsonrpc STATIC
    src/jsonrpc/error.cpp
    src/jsonrpc/method.cpp
    src/jsonrpc/properties.cpp
    src/jsonrpc/service.cpp
)

target_link_libraries(jsonrpc
    ${libnode-deps}
)

set_target_properties(jsonrpc PROPERTIES
    COMPILE_FLAGS "${libnode-cflags}"
)

# build tests ------------------------------------------------------------------

if(LIBNODE_BUILD_TEST)
    add_subdirectory(gtest)
endif(LIBNODE_BUILD_TEST)
