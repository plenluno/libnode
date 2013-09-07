// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/detail/http/header_scanner.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

TEST(GTestHttpHeaderScanner, TestScanHeaderField) {
    ASSERT_TRUE(scanHeaderField(NULL, 0)->isEmpty());
    ASSERT_TRUE(scanHeaderField("Content-Length", 0)->isEmpty());

    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField("Content-Length", 14));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField("CONTENT-LENGTH", 14));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField("content-length", 14));

    // ignore length parameters
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField("Content-Length", 1));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField("Content-Lengths", 15));

    ASSERT_TRUE(scanHeaderField("X-Header-Header-Header-Header-Header", 8)
        ->equals(String::create("x-header")));

    ASSERT_TRUE(scanHeaderField("X-Header-Header-Header-Header-Header", 36)
        ->equals(String::create("x-header-header-header-header-header")));
}

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj
