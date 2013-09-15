# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(gtest)

set(libnode-test-src
    gtest_main.cpp
    gtest_buffer.cpp
    gtest_common.cpp
    gtest_dgram.cpp
#   gtest_dns.cpp
    gtest_event_emitter.cpp
    gtest_free_list.cpp
    gtest_fs.cpp
    gtest_http_common.cpp
    gtest_http_echo.cpp
    gtest_http_echo_old.cpp
    gtest_http_header_scanner.cpp
    gtest_http_static.cpp
    gtest_http_status.cpp
    gtest_net_pipe.cpp
    gtest_net_tcp.cpp
    gtest_os.cpp
    gtest_path.cpp
    gtest_process.cpp
    gtest_querystring.cpp
    gtest_timer.cpp
    gtest_url.cpp
    gtest_util.cpp
    gtest_uv_error.cpp
)

if(LIBNODE_USE_CRYPTO)
    set(libnode-test-src
        ${libnode-test-src}
        gtest_crypto_cipher.cpp
        gtest_crypto_hash.cpp
#       gtest_crypto_rsa.cpp
    )
endif(LIBNODE_USE_CRYPTO)

if(LIBNODE_USE_CXX11)
    set(libnode-test-src
        ${libnode-test-src}
        gtest_http_echo_cxx11.cpp
    )
endif(LIBNODE_USE_CXX11)

if(LIBNODE_USE_THREAD)
    set(libnode-test-src
        ${libnode-test-src}
        gtest_async.cpp
        gtest_message_queue.cpp
    )
endif(LIBNODE_USE_THREAD)

add_executable(libnode-test
    ${libnode-test-src}
)

target_link_libraries(libnode-test
    ${libnode-linklibs}
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
