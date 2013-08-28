# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(example)

# gflags
include_directories(
    deps/gflags/include
)

add_subdirectory(deps/gflags)

# hello-server
add_executable(hello-server
    hello_server.cpp
)

target_link_libraries(hello-server
    ${libnode-linklibs}
)

if(APPLE)
    set_target_properties(hello-server PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(hello-server PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)

# http-client
add_executable(http-client
    http_client.cpp
)

target_link_libraries(http-client
    ${libnode-linklibs}
    gflags
)

if(APPLE)
    set_target_properties(http-client PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(http-client PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)
