# Copyright (c) 2012-2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(libnode)

include(CMakeDependentOption)

# build type -------------------------------------------------------------------

message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
    message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
endif(CMAKE_BUILD_TYPE STREQUAL "")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "CMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}")
    message(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "CMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}")
    message(STATUS "CMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# options ----------------------------------------------------------------------

option(LIBNODE_DEBUG           "Debug Mode"        OFF)
option(LIBNODE_TRACE           "Trace Mode"        OFF)
option(LIBNODE_USE_BDWGC       "Use BDW-GC"        OFF)
option(LIBNODE_USE_CXX11       "Use C++11"         OFF)
option(LIBNODE_USE_THREAD      "Use Threads"       OFF)
option(LIBNODE_USE_UTF32       "Use UTF32"         OFF)
option(LIBNODE_USE_SSL         "Use SSL"           OFF)
option(LIBNODE_USE_CRYPTO      "Use Crypto"        ON)
option(LIBNODE_BUILD_TEST      "Build Tests"       OFF)
option(LIBNODE_REMOVE_LISTENER "Remove Listeners"  ON)

## internal use
option(SUBMODULE_LIBJ          "Submodule libj"    ON)
option(SUBMODULE_OPENSSL       "Submodule OpenSSL" ON)

cmake_dependent_option(LIBJ_DEBUG
    "Debug Mode" ON
    "LIBNODE_DEBUG" OFF
)
cmake_dependent_option(LIBJ_TRACE
    "Trace Mode" ON
    "LIBNODE_TRACE" OFF
)
cmake_dependent_option(LIBJ_USE_BDWGC
    "Use BDW-GC" ON
    "LIBNODE_USE_BDWGC" OFF
)
cmake_dependent_option(LIBJ_USE_CXX11
    "Use C++11" ON
    "LIBNODE_USE_CXX11" OFF
)
cmake_dependent_option(LIBJ_USE_THREAD
    "Use Threads" ON
    "LIBNODE_USE_THREAD" OFF
)
cmake_dependent_option(LIBJ_BUILD_GTEST
    "Build Google Test" ON
    "LIBNODE_BUILD_TEST" OFF
)
cmake_dependent_option(LIBJ_USE_UTF32
    "Use UTF32" ON
    "LIBNODE_USE_UTF32" OFF
)
cmake_dependent_option(OPENSSL_BUILD_LIBSSL
    "Build libssl" ON
    "LIBNODE_USE_SSL" OFF
)
cmake_dependent_option(OPENSSL_BUILD_LIBCRYPTO
    "Build libcrypto" ON
    "LIBNODE_USE_CRYPTO" OFF
)

## check
if(NOT SUBMODULE_LIBJ)
    message(FATAL_ERROR "SUBMODULE_LIBJ=OFF")
endif(NOT SUBMODULE_LIBJ)

if(NOT SUBMODULE_OPENSSL)
    message(FATAL_ERROR "SUBMODULE_OPENSSL=OFF")
endif(NOT SUBMODULE_OPENSSL)

if(NOT LIBNODE_DEBUG STREQUAL LIBJ_DEBUG)
    message(FATAL_ERROR "LIBNODE_DEBUG != LIBJ_DEBUG")
endif(NOT LIBNODE_DEBUG STREQUAL LIBJ_DEBUG)

if(NOT LIBNODE_TRACE STREQUAL LIBJ_TRACE)
    message(FATAL_ERROR "LIBNODE_TRACE != LIBJ_TRACE")
endif(NOT LIBNODE_TRACE STREQUAL LIBJ_TRACE)

if(NOT LIBNODE_USE_BDWGC STREQUAL LIBJ_USE_BDWGC)
    message(FATAL_ERROR "LIBNODE_USE_BDWGC != LIBJ_USE_BDWGC")
endif(NOT LIBNODE_USE_BDWGC STREQUAL LIBJ_USE_BDWGC)

if(NOT LIBNODE_USE_CXX11 STREQUAL LIBJ_USE_CXX11)
    message(FATAL_ERROR "LIBNODE_USE_CXX11 != LIBJ_USE_CXX11")
endif(NOT LIBNODE_USE_CXX11 STREQUAL LIBJ_USE_CXX11)

if(NOT LIBNODE_USE_THREAD STREQUAL LIBJ_USE_THREAD)
    message(FATAL_ERROR "LIBNODE_USE_THREAD != LIBJ_USE_THREAD")
endif(NOT LIBNODE_USE_THREAD STREQUAL LIBJ_USE_THREAD)

if(NOT LIBNODE_USE_UTF32 STREQUAL LIBJ_USE_UTF32)
    message(FATAL_ERROR "LIBNODE_USE_UTF32 != LIBJ_USE_UTF32")
endif(NOT LIBNODE_USE_UTF32 STREQUAL LIBJ_USE_UTF32)

if(LIBNODE_TRACE AND NOT LIBNODE_DEBUG)
    message(FATAL_ERROR "LIBNODE_TRACE=ON but LIBNODE_DEBUG=OFF")
endif(LIBNODE_TRACE AND NOT LIBNODE_DEBUG)

if(LIBNODE_TRACE AND LIBNODE_USE_THREAD)
    message(FATAL_ERROR "LIBNODE_TRACE=ON but LIBNODE_USE_THREAD=ON")
endif(LIBNODE_TRACE AND LIBNODE_USE_THREAD)

if(LIBNODE_USE_BDWGC AND LIBNODE_USE_THREAD)
    message(FATAL_ERROR "LIBNODE_USE_BDWGC=ON but LIBNODE_USE_THREAD=ON")
endif(LIBNODE_USE_BDWGC AND LIBNODE_USE_THREAD)

## status
message(STATUS "LIBNODE_DEBUG=${LIBNODE_DEBUG}")
message(STATUS "LIBNODE_TRACE=${LIBNODE_TRACE}")
message(STATUS "LIBNODE_USE_BDWGC=${LIBNODE_USE_BDWGC}")
message(STATUS "LIBNODE_USE_CXX11=${LIBNODE_USE_CXX11}")
message(STATUS "LIBNODE_USE_THREAD=${LIBNODE_USE_THREAD}")
message(STATUS "LIBNODE_USE_UTF32=${LIBNODE_USE_UTF32}")
message(STATUS "LIBNODE_USE_SSL=${LIBNODE_USE_SSL}")
message(STATUS "LIBNODE_USE_CRYPTO=${LIBNODE_USE_CRYPTO}")
message(STATUS "LIBNODE_BUILD_TEST=${LIBNODE_BUILD_TEST}")
message(STATUS "LIBNODE_REMOVE_LISTENER=${LIBNODE_REMOVE_LISTENER}")

# variables --------------------------------------------------------------------

## libnode-include
set(libnode-include
    include
    deps/http-parser
    deps/b64/include
    deps/libj/include
    deps/libuv/include
    deps/libj/deps/boost-svn
)

if(LIBNODE_USE_BDWGC)
    set(libnode-include
        ${libnode-include}
        deps/libj/deps/bdwgc/include
    )
endif(LIBNODE_USE_BDWGC)

if(LIBNODE_USE_SSL OR LIBNODE_USE_CRYPTO)
    set(libnode-include
        ${libnode-include}
        deps/openssl/openssl/include
    )
endif(LIBNODE_USE_SSL OR LIBNODE_USE_CRYPTO)

if(LIBNODE_BUILD_TEST)
    set(libnode-include
        ${libnode-include}
        deps/libj/deps/googletest
        deps/libj/deps/googletest/include
    )
endif(LIBNODE_BUILD_TEST)

## libnode-src
set(libnode-src
    src/buffer.cpp
    src/dns.cpp
    src/events/event_emitter.cpp
    src/fs.cpp
    src/fs/stats.cpp
    src/http.cpp
    src/http/agent.cpp
    src/http/client.cpp
    src/http/header.cpp
    src/http/method.cpp
    src/http/option.cpp
    src/http/server.cpp
    src/http/status.cpp
    src/net.cpp
    src/net/option.cpp
    src/net/server.cpp
    src/net/socket.cpp
    src/node.cpp
    src/path.cpp
    src/process.cpp
    src/querystring.cpp
    src/stream.cpp
    src/string_decoder.cpp
    src/timer.cpp
    src/url.cpp
    src/util.cpp
    src/uv/error.cpp
)

if(LIBNODE_USE_CRYPTO)
    set(libnode-src
        ${libnode-src}
        src/crypto.cpp
        src/crypto/cipher.cpp
        src/crypto/decipher.cpp
        src/crypto/hash.cpp
        src/crypto/rsa/cipher.cpp
        src/crypto/rsa/decipher.cpp
        src/crypto/rsa/key_pair.cpp
    )
endif(LIBNODE_USE_CRYPTO)

if(LIBNODE_USE_THREAD)
    set(libnode-src
        ${libnode-src}
        src/async.cpp
        src/message_queue.cpp
    )
endif(LIBNODE_USE_THREAD)

## libnode-deps
set(libnode-deps
    httpparser
    j
    uv
)

if(UNIX AND NOT ANDROID)
    set(libnode-deps
        ${libnode-deps}
        pthread
    )
endif(UNIX AND NOT ANDROID)

if(LIBNODE_USE_SSL)
    set(libnode-deps
        ${libnode-deps}
        ssl
    )
endif(LIBNODE_USE_SSL)

if(LIBNODE_USE_CRYPTO)
    set(libnode-deps
        ${libnode-deps}
        crypto
    )
else(LIBNODE_USE_CRYPTO)
    set(libnode-deps
        ${libnode-deps}
        b64
    )
endif(LIBNODE_USE_CRYPTO)

if(UXIX AND NOT APPLE)
    set(libnode-deps
        ${libnode-deps}
        rt
    )
endif(UXIX AND NOT APPLE)

## libnode-linklibs
set(libnode-linklibs
    node
)

if(LIBNODE_BUILD_TEST)
    set(libnode-linklibs
        ${libnode-linklibs}
        gtest
    )
endif(LIBNODE_BUILD_TEST)

if(LIBNODE_USE_CXX11 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(libnode-linklibs
        ${libnode-linklibs}
        c++
        c++abi
    )
endif(LIBNODE_USE_CXX11 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

## libnode-cxx11-cflags
set(libnode-cxx11-cflags
    --std=c++0x
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(libnode-cxx11-cflags
        ${libnode-cxx11-cflags}
        -stdlib=libc++
    )
endif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

## libnode-cflags
if(UNIX)
    set(libnode-cflags
        -Wall
        -fno-rtti
        -fno-exceptions
        -D_FILE_OFFSET_BITS=64
    )

    if(LIBNODE_USE_CXX11)
        set(libnode-cflags
            ${libnode-cflags}
            ${libnode-cxx11-cflags}
        )
    endif(LIBNODE_USE_CXX11)

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(libnode-cflags
            ${libnode-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wno-unused-private-field
        )
    endif(CMAKE_BUILD_TYPE STREQUAL "Release")

    if(LIBNODE_TRACE)
        set(libnode-cflags
            ${libnode-cflags}
            -finstrument-functions   
        )
    endif(LIBNODE_TRACE)
endif(UNIX)

string(REPLACE ";" " " libnode-cflags "${libnode-cflags}")

## libnode-test-cflags
if(UNIX)
    set(libnode-test-cflags
        -Wno-unused-value
    )

    if(LIBNODE_USE_CXX11)
        set(libnode-test-cflags
            ${libnode-test-cflags}
            ${libnode-cxx11-cflags}
        )
    endif(LIBNODE_USE_CXX11)

    if(LIBNODE_TRACE)
        set(libnode-test-cflags
            ${libnode-test-cflags}
            -finstrument-functions   
        )
    endif(LIBNODE_TRACE)
else(UNIX)
    set(libnode-test-cflags
        /D_VARIADIC_MAX=10
    )
endif(UNIX)

string(REPLACE ";" " " libnode-test-cflags "${libnode-test-cflags}")

# configure --------------------------------------------------------------------

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libnode/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libnode/config.h )

# build libraries --------------------------------------------------------------

include_directories(
    ${libnode-include}
)

link_directories(
    ${CMAKE_BINARY_DIR}
)

## libj
add_subdirectory(deps/libj)

## libuv
add_subdirectory(deps/libuv)

## b64
if(NOT LIBNODE_USE_CRYPTO)
    add_subdirectory(deps/b64)
endif(NOT LIBNODE_USE_CRYPTO)

## http-parser
add_subdirectory(deps/http-parser)

## openssl
if(LIBNODE_USE_SSL OR LIBNODE_USE_CRYPTO)
    add_subdirectory(deps/openssl)
endif(LIBNODE_USE_SSL OR LIBNODE_USE_CRYPTO)

## libnode
add_library(node STATIC
    ${libnode-src}
)

target_link_libraries(node
    ${libnode-deps}
)

set_target_properties(node PROPERTIES
    COMPILE_FLAGS "${libnode-cflags}"
)

# build tests ------------------------------------------------------------------

if(LIBNODE_BUILD_TEST)
    add_subdirectory(gtest)
endif(LIBNODE_BUILD_TEST)

# build example ----------------------------------------------------------------

add_subdirectory(example)
