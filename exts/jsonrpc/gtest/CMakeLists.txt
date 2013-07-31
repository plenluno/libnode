# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(jsonrpc-test)

add_executable(jsonrpc-test
    gtest_jsonrpc.cpp
    gtest_main.cpp
)

target_link_libraries(jsonrpc-test
    ${libnode-linklibs}
    jsonrpc
)

if(APPLE)
    set_target_properties(jsonrpc-test PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(jsonrpc-test PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)
