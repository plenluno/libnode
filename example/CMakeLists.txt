# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(example)

if(LIBNODE_USE_CXX11)
    add_executable(libnode-hello-server
        hello_server_cxx11.cpp
    )
else(LIBNODE_USE_CXX11)
    add_executable(libnode-hello-server
        hello_server.cpp
    )
endif(LIBNODE_USE_CXX11)

target_link_libraries(libnode-hello-server
    ${libnode-linklibs}
)

if(APPLE)
    set_target_properties(libnode-hello-server PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(libnode-hello-server PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)
