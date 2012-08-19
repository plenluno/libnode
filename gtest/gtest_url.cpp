// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

namespace libj {
namespace node {

TEST(GTestUrl, TestParse1) {
    String::CPtr urlStr = String::create("/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_TRUE(url->getCPtr<String>(url::HREF)->equals(urlStr));
    ASSERT_TRUE(url->getCPtr<String>(url::PATHNAME)->equals(
        String::create("/foo/bar")));
    ASSERT_TRUE(url->getCPtr<String>(url::QUERY)->equals(
        String::create("abc=123&pqr=xyz")));
    ASSERT_TRUE(url->getCPtr<String>(url::HASH)->equals(
        String::create("#fr")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATH)->equals(
        String::create("/foo/bar?abc=123&pqr=xyz")));
}

TEST(GTestUrl, TestParse2) {
    String::CPtr urlStr = String::create(
        "http://user1:passwd2@www.gtest.com:8888/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_TRUE(url->getCPtr<String>(url::HREF)->equals(urlStr));
    ASSERT_TRUE(url->getCPtr<String>(url::PROTOCOL)->equals(
        String::create("http")));
    ASSERT_TRUE(url->getCPtr<String>(url::HOST)->equals(
        String::create("www.gtest.com:8888")));
    ASSERT_TRUE(url->getCPtr<String>(url::AUTH)->equals(
        String::create("user1:passwd2")));
    ASSERT_TRUE(url->getCPtr<String>(url::HOSTNAME)->equals(
        String::create("www.gtest.com")));
    ASSERT_TRUE(url->getCPtr<String>(url::PORT)->equals(
        String::create("8888")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATHNAME)->equals(
        String::create("/foo/bar")));
    ASSERT_TRUE(url->getCPtr<String>(url::QUERY)->equals(
        String::create("abc=123&pqr=xyz")));
    ASSERT_TRUE(url->getCPtr<String>(url::HASH)->equals(
        String::create("#fr")));
    ASSERT_TRUE(url->getCPtr<String>(url::PATH)->equals(
        String::create("/foo/bar?abc=123&pqr=xyz")));
}

}  // namespace node
}  // namespace libj
