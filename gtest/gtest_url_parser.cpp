// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libj/string.h>
#include <url_parser.h>

namespace libj {

TEST(GTestUrlParser, TestParseUrl1) {
    struct parsed_url* url = parse_url("/foo/bar?abc=123&pqr=xyz");
    ASSERT_FALSE(url->scheme);
    ASSERT_FALSE(url->host);
    ASSERT_FALSE(url->port);
    ASSERT_EQ(String::create(url->path)->compareTo(String::create("foo/bar")), 0);
    ASSERT_EQ(String::create(url->query)->compareTo(String::create("abc=123&pqr=xyz")), 0);
    parsed_url_free(url);
}

TEST(GTestUrlParser, TestParseUrl2) {
    struct parsed_url* url = parse_url("http://www.gtest.com:8888/foo/bar?abc=123&pqr=xyz");
    ASSERT_EQ(String::create(url->scheme)->compareTo(String::create("http")), 0);
    ASSERT_EQ(String::create(url->host)->compareTo(String::create("www.gtest.com")), 0);
    ASSERT_EQ(String::create(url->port)->compareTo(String::create("8888")), 0);
    ASSERT_EQ(String::create(url->path)->compareTo(String::create("foo/bar")), 0);
    ASSERT_EQ(String::create(url->query)->compareTo(String::create("abc=123&pqr=xyz")), 0);
    parsed_url_free(url);
}

}  // namespace libj
