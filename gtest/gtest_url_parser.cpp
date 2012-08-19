// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libj/string.h>
#include <url_parser.h>

namespace libj {

TEST(GTestUrlParser, TestParseUrl1) {
    struct parsed_url* url = parse_url("/foo/bar?abc=123&pqr=xyz#fr");
    ASSERT_FALSE(url->scheme);
    ASSERT_FALSE(url->host);
    ASSERT_FALSE(url->port);
    ASSERT_TRUE(String::create(url->path)->equals(
        String::create("foo/bar")));
    ASSERT_TRUE(String::create(url->query)->equals(
        String::create("abc=123&pqr=xyz")));
    ASSERT_TRUE(String::create(url->fragment)->equals(
        String::create("fr")));
    parsed_url_free(url);
}

TEST(GTestUrlParser, TestParseUrl2) {
    struct parsed_url* url = parse_url(
        "http://user1:passwd2@www.gtest.com:8888/foo/bar?abc=123&pqr=xyz#fr");
    ASSERT_TRUE(String::create(url->scheme)->equals(
        String::create("http")));
    ASSERT_TRUE(String::create(url->host)->equals(
        String::create("www.gtest.com")));
    ASSERT_TRUE(String::create(url->port)->equals(
        String::create("8888")));
    ASSERT_TRUE(String::create(url->path)->equals(
        String::create("foo/bar")));
    ASSERT_TRUE(String::create(url->query)->equals(
        String::create("abc=123&pqr=xyz")));
    ASSERT_TRUE(String::create(url->username)->equals(
        String::create("user1")));
    ASSERT_TRUE(String::create(url->password)->equals(
        String::create("passwd2")));
    ASSERT_TRUE(String::create(url->fragment)->equals(
        String::create("fr")));
    parsed_url_free(url);
}

}  // namespace libj
