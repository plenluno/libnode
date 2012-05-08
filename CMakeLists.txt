cmake_minimum_required(VERSION 2.8)

project(libnode)

find_package(Boost)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found.")
endif()

find_library(GC gc REQUIRED)
if(NOT EXISTS ${GC})
    message(FATAL_ERROR "libgc not found.")
endif()

find_library(PTHREAD pthread REQUIRED)
if(NOT EXISTS ${PTHREAD})
    message(FATAL_ERROR "libpthread not found.")
endif()

find_library(GTEST gtest REQUIRED)
if(NOT EXISTS ${GTEST})
    message(FATAL_ERROR "libgtest not found.")
endif()

include_directories(
    include
    deps
    deps/http-parser
    deps/url_parser
    deps/libj/include
    deps/libj/deps
    deps/libuv/include
)

link_directories(
    ${CMAKE_BINARY_DIR}
)

set(libnode-src
    src/event_emitter.cpp
    src/http_server.cpp
    src/http_server_request.cpp
    src/http_server_request_impl.cpp
    src/http_server_response.cpp
    src/http_server_response_impl.cpp
    src/js_object.cpp
    src/node.cpp
    src/url.cpp
)

set(libnode-common-deps
    parser-http
    parser-url
    uv
)

set(libnode-deps-with-sp
    j-with-sp
    ${libnode-common-deps}
)

set(libnode-deps-with-bdw-gc
    j-with-bdw-gc
    ${libnode-common-deps}
)

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

# gtest
add_executable(libnode-gtest
    gtest/gtest_main.cpp
    gtest/gtest_event_emitter.cpp
    gtest/gtest_http_server.cpp
    gtest/gtest_url_parser.cpp
    ${libnode-src}
)

target_link_libraries(libnode-gtest
    gtest
    pthread
    ${libnode-deps-with-sp}
)

set_target_properties(libnode-gtest
    PROPERTIES COMPILE_FLAGS "-DLIBJ_GTEST_BUILD_ERRORS")

# libnode with shared_ptr
add_library(node-with-sp STATIC
    ${libnode-src}
)

target_link_libraries(node-with-sp
    ${libnode-deps-with-sp}
)

set_target_properties(node-with-sp
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions")

# libnode with bdw gc
add_library(node-with-bdw-gc STATIC
    ${libnode-src}
)

target_link_libraries(node-with-bdw-gc
    ${libnode-deps-with-bdw-gc}
)

set_target_properties(node-with-bdw-gc
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions -DLIBJ_USE_BOEHM_GC")

# echo server with shared_ptr
add_executable(echo-server-with-sp
    sample/echo_server.cpp
)

target_link_libraries(echo-server-with-sp
    node-with-sp
    ${libnode-deps-with-sp}
)

set_target_properties(echo-server-with-sp
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions")

# echo server with bdw gc
add_executable(echo-server-with-bdw-gc
    sample/echo_server.cpp
)

target_link_libraries(echo-server-with-bdw-gc
    node-with-bdw-gc
    ${libnode-deps-with-bdw-gc}
)

set_target_properties(echo-server-with-bdw-gc
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions -DLIBJ_USE_BOEHM_GC")
