# Copyright (c) 2012-2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(libnode)

include(CMakeDependentOption)

# options ----------------------------------------------------------------------

option(LIBNODE_USE_BDWGC       "Use BDW-GC"       OFF)
option(LIBNODE_USE_CXX11       "Use C++11"        OFF)
option(LIBNODE_USE_SSL         "Use SSL"          OFF)
option(LIBNODE_USE_CRYPTO      "Use Crypto"       ON)
option(LIBNODE_USE_THREAD      "Use Threads"      OFF)
option(LIBNODE_USE_UTF32       "Use UTF32"        OFF)
option(LIBNODE_BUILD_TEST      "Build Tests"      OFF)
option(LIBNODE_REMOVE_LISTENER "Remove Listeners" ON)

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
cmake_dependent_option(LIBJ_USE_UTF32
    "Use UTF32" ON
    "LIBNODE_USE_UTF32" OFF
)
cmake_dependent_option(LIBNODE_BUILD_OPENSSL
    "Build OpenSSL" ON
    "LIBNODE_USE_SSL OR LIBNODE_USE_CRYPTO" OFF
)
cmake_dependent_option(OPENSSL_BUILD_LIBSSL
    "Build libssl" ON
    "LIBNODE_USE_SSL" OFF
)
cmake_dependent_option(OPENSSL_BUILD_LIBCRYPTO
    "Build libcrypto" ON
    "LIBNODE_USE_CRYPTO" OFF
)

message(STATUS "LIBNODE_USE_BDWGC=${LIBNODE_USE_BDWGC}")
message(STATUS "LIBNODE_USE_CXX11=${LIBNODE_USE_CXX11}")
message(STATUS "LIBNODE_USE_SSL=${LIBNODE_USE_SSL}")
message(STATUS "LIBNODE_USE_CRYPTO=${LIBNODE_USE_CRYPTO}")
message(STATUS "LIBNODE_USE_THREAD=${LIBNODE_USE_THREAD}")
message(STATUS "LIBNODE_USE_UTF32=${LIBNODE_USE_UTF32}")
message(STATUS "LIBNODE_BUILD_TEST=${LIBNODE_BUILD_TEST}")
message(STATUS "LIBNODE_BUILD_OPENSSL=${LIBNODE_BUILD_OPENSSL}")
message(STATUS "LIBNODE_REMOVE_LISTENER=${LIBNODE_REMOVE_LISTENER}")

# variables --------------------------------------------------------------------

set(CMAKE_CXX_FLAGS_RELEASE
    "-Wno-unused-variable"
)

## libnode-include
set(libnode-include
    include
    deps/http-parser
    deps/b64/include
    deps/libj/include
    deps/libuv/include
    deps/libj/deps/boost-svn
)

if(LIBNODE_BUILD_OPENSSL)
    set(libnode-include
        ${libnode-include}
        deps/openssl/openssl/include
    )
endif(LIBNODE_BUILD_OPENSSL)

if(LIBJ_BUILD_GTEST)
    set(libnode-include
        ${libnode-include}
        deps/libj/deps/googletest/include
    )
endif(LIBJ_BUILD_GTEST)

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

if(UNIX)
    set(libnode-deps
        ${libnode-deps}
        pthread
    )
endif(UNIX)

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
            --std=c++0x
        )
    endif(LIBNODE_USE_CXX11)
endif(UNIX)

string(REPLACE ";" " " libnode-cflags "${libnode-cflags}")

## libnode-test-src
set(libnode-test-src
    gtest/gtest_main.cpp
    gtest/gtest_buffer.cpp
    gtest/gtest_common.cpp
#   gtest/gtest_dns.cpp
    gtest/gtest_event_emitter.cpp
    gtest/gtest_http_common.cpp
    gtest/gtest_http_echo.cpp
    gtest/gtest_http_echo_old.cpp
    gtest/gtest_http_static.cpp
    gtest/gtest_http_status.cpp
#   gtest/gtest_net_pipe_echo.cpp
    gtest/gtest_net_tcp_echo.cpp
    gtest/gtest_path.cpp
    gtest/gtest_process.cpp
    gtest/gtest_querystring.cpp
    gtest/gtest_timer.cpp
    gtest/gtest_url.cpp
    gtest/gtest_util.cpp
    gtest/gtest_uv_error.cpp
)

if(LIBNODE_USE_CRYPTO)
    set(libnode-test-src
        ${libnode-test-src}
        gtest/gtest_crypto_cipher.cpp
        gtest/gtest_crypto_hash.cpp
#       gtest/gtest_crypto_rsa.cpp
    )
endif(LIBNODE_USE_CRYPTO)

if(LIBNODE_USE_CXX11)
    set(libnode-test-src
        ${libnode-test-src}
        gtest/gtest_http_echo_cxx11.cpp
    )
endif(LIBNODE_USE_CXX11)

if(LIBNODE_USE_THREAD)
    set(libnode-test-src
        ${libnode-test-src}
        gtest/gtest_async.cpp
        gtest/gtest_message_queue.cpp
    )
endif(LIBNODE_USE_THREAD)

## libnode-test-cflags
if(UNIX)
    set(libnode-test-cflags
        -Wall
    )

    if(LIBNODE_USE_CXX11)
        set(libnode-test-cflags
            ${libnode-test-cflags}
            --std=c++0x
        )
    endif(LIBNODE_USE_CXX11)
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

## b64
if(NOT LIBNODE_USE_CRYPTO)
    add_library(b64
        deps/b64/src/b64.c
    )
endif(NOT LIBNODE_USE_CRYPTO)

## http-parser
add_library(httpparser
    deps/http-parser/http_parser.c
)

## libuv
add_subdirectory(deps/libuv)

## openssl
if(LIBNODE_BUILD_OPENSSL)
    add_subdirectory(deps/openssl)
endif(LIBNODE_BUILD_OPENSSL)

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

# build gtests -----------------------------------------------------------------

if(LIBNODE_BUILD_TEST)
    add_executable(libnode-test
        ${libnode-test-src}
    )

    target_link_libraries(libnode-test
        ${libnode-deps}
        gtest
        node
    )

    if(APPLE)
        set_target_properties(libnode-test PROPERTIES
            COMPILE_FLAGS "${libnode-test-cflags}"
            LINK_FLAGS "-framework CoreServices"
        )
    else(APPLE)
        set_target_properties(libnode-test PROPERTIES
            COMPILE_FLAGS "${libnode-test-cflags}"
        )
    endif(APPLE)
endif(LIBNODE_BUILD_TEST)
