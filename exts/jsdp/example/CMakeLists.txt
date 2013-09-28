# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(jsdp-discover)

add_executable(jsdp-discover
    discover.cpp
)

target_link_libraries(jsdp-discover
    jsdp
    ${libnode-linklibs}
)

if(APPLE)
    set_target_properties(jsdp-discover PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(jsdp-discover PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)
