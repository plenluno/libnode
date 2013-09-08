// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/detail/http/header_scanner.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

TEST(GTestHttpHeaderScanner, TestScanHeaderField) {
    ASSERT_TRUE(scanHeaderField<char>(NULL, 0)->isEmpty());
    ASSERT_TRUE(scanHeaderField<char>("Content-Length", 0)->isEmpty());

    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<char>("Content-Length", 14));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<char>("CONTENT-LENGTH", 14));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<char>("content-length", 14));

    // ignore length parameters
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<char>("Content-Length", 1));
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<char>("Content-Lengths", 15));

    ASSERT_TRUE(
        scanHeaderField<char>("X-Header-Header-Header-Header-Header", 8)
            ->equals(String::create("x-header")));

    ASSERT_TRUE(
        scanHeaderField<char>("X-Header-Header-Header-Header-Header", 36)
            ->equals(String::create("x-header-header-header-header-header")));
}

TEST(GTestHttpHeaderScanner, TestScanHeaderField2) {
    String::CPtr field = String::create("Content-Length");
    ASSERT_EQ(
        node::http::LHEADER_CONTENT_LENGTH,
        scanHeaderField<Char>(field->data(), field->length()));

    field = String::create("X-Header-Header-Header-Header-Header");
    ASSERT_TRUE(
        scanHeaderField<Char>(field->data(), field->length())
            ->equals(String::create("x-header-header-header-header-header")));
}

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj
