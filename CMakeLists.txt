cmake_minimum_required(VERSION 2.8)

project(libnode)

# options ------------------------------------------------------------------------------------------

option(LIBNODE_BUILD_GTEST "Build Google Tests" OFF)
option(LIBNODE_BUILD_OPENSSL "Build OpenSSL" ON)
option(LIBNODE_BUILD_SAMPLE "Build Samples" OFF)

message(STATUS "LIBNODE_BUILD_GTEST=${LIBNODE_BUILD_GTEST}")
message(STATUS "LIBNODE_BUILD_OPENSSL=${LIBNODE_BUILD_OPENSSL}")
message(STATUS "LIBNODE_BUILD_SAMPLE=${LIBNODE_BUILD_SAMPLE}")

# find libraries -----------------------------------------------------------------------------------

find_library(PTHREAD pthread REQUIRED)
if(NOT EXISTS ${PTHREAD})
    message(FATAL_ERROR "libpthread not found.")
endif()

if(LIBNODE_BUILD_GTEST)
    find_library(GTEST gtest REQUIRED)
    if(NOT EXISTS ${GTEST})
        message(FATAL_ERROR "libgtest not found.")
    endif()
endif(LIBNODE_BUILD_GTEST)

# set variables ------------------------------------------------------------------------------------

set(libnode-src
    src/buffer.cpp
    src/crypto.cpp
    src/crypto/hash.cpp
    src/error.cpp
    src/events/event_emitter.cpp
    src/fs.cpp
    src/fs/stats.cpp
    src/http.cpp
    src/http/header.cpp
    src/http/server.cpp
    src/http/server_response.cpp
    src/http/status.cpp
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
)

set(libnode-deps
    crypto
    httpparser
    j
    pthread
    urlparser
    uv
)

if(NOT APPLE)
    set(libnode-deps
        ${libnode-deps}
        rt
    )
endif(NOT APPLE)

set(libnode-include
    include
    deps
    deps/http-parser
    deps/url_parser
    deps/libj/include
    deps/libj/deps/boost-svn
    deps/libuv/include
)

if(LIBNODE_BUILD_OPENSSL)
    set(libnode-include
        ${libnode-include}
        deps/openssl/include
    )
endif(LIBNODE_BUILD_OPENSSL)

# build libraries ----------------------------------------------------------------------------------

include_directories(
    ${libnode-include}
)

link_directories(
    ${CMAKE_BINARY_DIR}
)

## libj
add_subdirectory(deps/libj)

# url_parser
add_library(urlparser
    deps/url_parser/url_parser.c
)

## http-parser
add_library(httpparser
    deps/http-parser/http_parser.c
)

## libuv
add_custom_command(
    TARGET httpparser PRE_BUILD
    COMMAND rsync -av ${CMAKE_CURRENT_LIST_DIR}/deps/libuv ${CMAKE_BINARY_DIR}/deps/
    COMMAND make -C ${CMAKE_BINARY_DIR}/deps/libuv
    COMMAND ln -sf ${CMAKE_BINARY_DIR}/deps/libuv/uv.a ${CMAKE_BINARY_DIR}/libuv.a
)

## openssl
if(LIBNODE_BUILD_OPENSSL)
    add_custom_command(
        TARGET httpparser PRE_BUILD
        COMMAND rsync -av ${CMAKE_CURRENT_LIST_DIR}/deps/openssl ${CMAKE_BINARY_DIR}/deps/
        COMMAND cd ${CMAKE_BINARY_DIR}/deps/openssl && ./config
    )
    if(APPLE)
        add_custom_command(
            TARGET httpparser PRE_BUILD
            COMMAND cd ${CMAKE_BINARY_DIR}/deps/openssl && ./Configure darwin64-x86_64-cc
        )
    endif(APPLE)
    add_custom_command(
        TARGET httpparser PRE_BUILD
        COMMAND make -C ${CMAKE_BINARY_DIR}/deps/openssl
        COMMAND ln -sf ${CMAKE_BINARY_DIR}/deps/openssl/libcrypto.a ${CMAKE_BINARY_DIR}/libcrypto.a
    )
endif(LIBNODE_BUILD_OPENSSL)

## libnode
add_library(node STATIC
    ${libnode-src}
)
target_link_libraries(node
    ${libnode-deps}
)
set_target_properties(node PROPERTIES
    COMPILE_FLAGS "-Wall -fno-rtti -fno-exceptions -D_FILE_OFFSET_BITS=64"
)

# build samples ------------------------------------------------------------------------------------

if(LIBNODE_BUILD_SAMPLE)

    set(libnode-sample-cflags
        "-Wall -fno-rtti -fno-exceptions"
    )
    set(libnode-sample-lflags
        "-framework CoreServices"
    )

## echo server
    add_executable(libnode-echo-server
        sample/echo_server.cpp
    )
    target_link_libraries(libnode-echo-server
        node
        ${libnode-deps}
    )
    if(APPLE)
        set_target_properties(libnode-echo-server PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
            LINK_FLAGS ${libnode-sample-lflags}
        )
    else(APPLE)
        set_target_properties(libnode-echo-server PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
        )
    endif(APPLE)

## static server
    add_executable(libnode-static-server
        sample/static_server.cpp
    )
    target_link_libraries(libnode-static-server
        node
        ${libnode-deps}
    )
    if(APPLE)
        set_target_properties(libnode-static-server PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
            LINK_FLAGS ${libnode-sample-lflags}
        )
    else(APPLE)
        set_target_properties(libnode-static-server PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
        )
    endif(APPLE)

## countdown timer
    add_executable(libnode-countdown-timer
        sample/countdown_timer.cpp
    )
    target_link_libraries(libnode-countdown-timer
        node
        ${libnode-deps}
    )
    if(APPLE)
        set_target_properties(libnode-countdown-timer PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
            LINK_FLAGS ${libnode-sample-lflags}
        )
    else(APPLE)
        set_target_properties(libnode-countdown-timer PROPERTIES
            COMPILE_FLAGS ${libnode-sample-cflags}
        )
    endif(APPLE)

endif(LIBNODE_BUILD_SAMPLE)

# build gtests -------------------------------------------------------------------------------------

if(LIBNODE_BUILD_GTEST)
    add_executable(libnode-gtest
        gtest/gtest_main.cpp
        gtest/gtest_buffer.cpp
        gtest/gtest_crypto_hash.cpp
        gtest/gtest_error.cpp
        gtest/gtest_event_emitter.cpp
        gtest/gtest_http_server.cpp
        gtest/gtest_http_status.cpp
        gtest/gtest_path.cpp
        gtest/gtest_querystring.cpp
        gtest/gtest_url.cpp
        gtest/gtest_url_parser.cpp
        gtest/gtest_util.cpp
        ${libnode-src}
    )
    target_link_libraries(libnode-gtest
        gtest
        ${libnode-deps}
    )
    if(APPLE)
        set_target_properties(libnode-gtest PROPERTIES
            COMPILE_FLAGS "-Wall"
            LINK_FLAGS "-framework CoreServices"
        )
    else(APPLE)
        set_target_properties(libnode-gtest PROPERTIES
            COMPILE_FLAGS "-Wall"
        )
    endif(APPLE)
endif(LIBNODE_BUILD_GTEST)
