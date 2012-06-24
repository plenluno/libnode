cmake_minimum_required(VERSION 2.8)

project(libnode)

option(LIBNODE_USE_GTEST "Use Google Test" OFF)

message(STATUS "LIBNODE_USE_GTEST=${LIBNODE_USE_GTEST}")

find_library(PTHREAD pthread REQUIRED)
if(NOT EXISTS ${PTHREAD})
    message(FATAL_ERROR "libpthread not found.")
endif()

if(LIBNODE_USE_GTEST)
    find_library(GTEST gtest REQUIRED)
    if(NOT EXISTS ${GTEST})
        message(FATAL_ERROR "libgtest not found.")
    endif()
endif(LIBNODE_USE_GTEST)

include_directories(
    include
    deps
    deps/http-parser
    deps/url_parser
    deps/libj/include
    deps/libj/deps/boost-svn
    deps/libuv/include
)

link_directories(
    ${CMAKE_BINARY_DIR}
)

set(libnode-src
    src/buffer.cpp
    src/event_emitter.cpp
    src/file_system.cpp
    src/http_server.cpp
    src/http_server_request.cpp
    src/http_server_request_impl.cpp
    src/http_server_response.cpp
    src/http_server_response_impl.cpp
    src/http_status.cpp
    src/node.cpp
    src/timer.cpp
    src/url.cpp
)

if(APPLE)
set(libnode-deps
    j
    parser-http
    parser-url
    pthread
    uv
)
else(APPLE)
set(libnode-deps
    j
    parser-http
    parser-url
    pthread
    uv
    rt
)
endif(APPLE)

# url_parser
add_library(parser-url
    deps/url_parser/url_parser.c
)

# http-parser
add_library(parser-http
    deps/http-parser/http_parser.c
)

# libj
add_subdirectory(deps/libj)

# libuv
add_custom_command(
    TARGET parser-http PRE_BUILD
    COMMAND rsync -av ${CMAKE_CURRENT_LIST_DIR}/deps/libuv ${CMAKE_BINARY_DIR}/deps/
    COMMAND make -C ${CMAKE_BINARY_DIR}/deps/libuv
    COMMAND ln -sf ${CMAKE_BINARY_DIR}/deps/libuv/uv.a ${CMAKE_BINARY_DIR}/libuv.a
)

# libnode
add_library(node STATIC
    ${libnode-src}
)
target_link_libraries(node
    ${libnode-deps}
)
set_target_properties(node
    PROPERTIES COMPILE_FLAGS "-Wall -fno-rtti -fno-exceptions")

# echo server
add_executable(echo-server
    sample/echo_server.cpp
)
target_link_libraries(echo-server
    node
    ${libnode-deps}
)
set_target_properties(echo-server
    PROPERTIES COMPILE_FLAGS "-Wall -fno-rtti -fno-exceptions")

# static server
add_executable(static-server
    sample/static_server.cpp
)
target_link_libraries(static-server
    node
    ${libnode-deps}
)
set_target_properties(static-server
    PROPERTIES COMPILE_FLAGS "-Wall -fno-rtti -fno-exceptions")

# countdown timer
add_executable(countdown-timer
    sample/countdown_timer.cpp
)
target_link_libraries(countdown-timer
    node
    ${libnode-deps}
)
set_target_properties(countdown-timer
    PROPERTIES COMPILE_FLAGS "-Wall -fno-rtti -fno-exceptions")

# gtest
if(LIBNODE_USE_GTEST)
    add_executable(libnode-gtest
        gtest/gtest_main.cpp
        gtest/gtest_buffer.cpp
        gtest/gtest_event_emitter.cpp
        gtest/gtest_http_server.cpp
        gtest/gtest_http_status.cpp
        gtest/gtest_url.cpp
        gtest/gtest_url_parser.cpp
        ${libnode-src}
    )
    target_link_libraries(libnode-gtest
        gtest
        ${libnode-deps}
    )
    set_target_properties(libnode-gtest
        PROPERTIES COMPILE_FLAGS "-Wall -DLIBJ_GTEST_BUILD_ERRORS")
endif(LIBNODE_USE_GTEST)
