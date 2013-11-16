# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(rsacrypto-test)

add_executable(rsacrypto-test
    gtest_crypto_rsa.cpp
    gtest_main.cpp
)

target_link_libraries(rsacrypto-test
    ${libnode-linklibs}
    rsacrypto
)

if(APPLE)
    set_target_properties(rsacrypto-test PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
        LINK_FLAGS "-framework CoreServices"
    )
else(APPLE)
    set_target_properties(rsacrypto-test PROPERTIES
        COMPILE_FLAGS "${libnode-test-cflags}"
    )
endif(APPLE)
