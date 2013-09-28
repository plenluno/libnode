# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(jsdp)

# build libraries --------------------------------------------------------------

add_library(jsdp STATIC
    src/jsdp.cpp
)

target_link_libraries(jsdp
    ${libnode-deps}
)

set_target_properties(jsdp PROPERTIES
    COMPILE_FLAGS "${libnode-cflags}"
)

# build examples ---------------------------------------------------------------

if(LIBNODE_BUILD_TEST)
    add_subdirectory(example)
endif(LIBNODE_BUILD_TEST)
