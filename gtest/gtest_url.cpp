// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

namespace libj {
namespace node {

TEST(GTestUrl, TestParse1) {
    String::CPtr urlStr = str("/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_TRUE(url->getCPtr<String>(url::HREF)->equals(urlStr));
    ASSERT_TRUE(url->getCPtr<String>(url::PATHNAME)->equals(
        str("/foo/bar")));
    ASSERT_TRUE(url->getCPtr<String>(url::QUERY)->equals(
        str("abc=123&pqr=xyz")));
    ASSERT_TRUE(url->getCPtr<String>(url::HASH)->equals(
        str("#fr")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATH)->equals(
        str("/foo/bar?abc=123&pqr=xyz")));
}

TEST(GTestUrl, TestParse2) {
    String::CPtr urlStr = str(
        "http://user1:passwd2@www.gtest.com:8888/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_TRUE(url->getCPtr<String>(url::HREF)->equals(urlStr));
    ASSERT_TRUE(url->getCPtr<String>(url::PROTOCOL)->equals(
        str("http:")));
    ASSERT_TRUE(url->getCPtr<String>(url::HOST)->equals(
        str("www.gtest.com:8888")));
    ASSERT_TRUE(url->getCPtr<String>(url::AUTH)->equals(
        str("user1:passwd2")));
    ASSERT_TRUE(url->getCPtr<String>(url::HOSTNAME)->equals(
        str("www.gtest.com")));
    ASSERT_TRUE(url->getCPtr<String>(url::PORT)->equals(
        str("8888")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATHNAME)->equals(
        str("/foo/bar")));
    ASSERT_TRUE(url->getCPtr<String>(url::QUERY)->equals(
        str("abc=123&pqr=xyz")));
    ASSERT_TRUE(url->getCPtr<String>(url::HASH)->equals(
        str("#fr")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATH)->equals(
        str("/foo/bar?abc=123&pqr=xyz")));
}

}  // namespace node
}  // namespace libj
