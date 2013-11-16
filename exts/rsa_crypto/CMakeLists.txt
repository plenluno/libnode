# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(rsacrypto)

# build libraries --------------------------------------------------------------

add_library(rsacrypto STATIC
    src/crypto/rsa/cipher.cpp
    src/crypto/rsa/decipher.cpp
    src/crypto/rsa/key_pair.cpp
)

target_link_libraries(rsacrypto
    node
    ${libnode-deps}
)

set_target_properties(rsacrypto PROPERTIES
    COMPILE_FLAGS "${libnode-cflags}"
)

# build tests ------------------------------------------------------------------

if(LIBNODE_BUILD_TEST)
    add_subdirectory(gtest)
endif(LIBNODE_BUILD_TEST)
