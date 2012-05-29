// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

namespace libj {
namespace node {

TEST(GTestUrl, TestParse1) {
    String::CPtr urlStr = String::create("/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_EQ(url->getCPtr<String>(url::HREF)
        ->compareTo(urlStr), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PATHNAME)
        ->compareTo(String::create("/foo/bar")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::QUERY)
        ->compareTo(String::create("abc=123&pqr=xyz")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::HASH)
        ->compareTo(String::create("#fr")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PATH)
        ->compareTo(String::create("/foo/bar?abc=123&pqr=xyz")), 0);
}

TEST(GTestUrl, TestParse2) {
    String::CPtr urlStr = String::create(
        "http://user1:passwd2@www.gtest.com:8888/foo/bar?abc=123&pqr=xyz#fr");
    JsObject::Ptr url = url::parse(urlStr);
    ASSERT_EQ(url->getCPtr<String>(url::HREF)
        ->compareTo(urlStr), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PROTOCOL)
        ->compareTo(String::create("http")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::HOST)
        ->compareTo(String::create("www.gtest.com:8888")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::AUTH)
        ->compareTo(String::create("user1:passwd2")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::HOSTNAME)
        ->compareTo(String::create("www.gtest.com")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PORT)
        ->compareTo(String::create("8888")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PATHNAME)
        ->compareTo(String::create("/foo/bar")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::QUERY)
        ->compareTo(String::create("abc=123&pqr=xyz")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::HASH)
        ->compareTo(String::create("#fr")), 0);
    ASSERT_EQ(url->getCPtr<String>(url::PATH)
        ->compareTo(String::create("/foo/bar?abc=123&pqr=xyz")), 0);
}

}  // namespace node
}  // namespace libj
