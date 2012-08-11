// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libj/json.h>
#include <libj/js_array.h>
#include <libj/null.h>
#include <libj/undefined.h>
#include <libnode/querystring.h>

namespace libj {
namespace node {

TEST(GTestQueryString, TestParse) {
    String::CPtr query = String::create();
    JsObject::Ptr obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create("{}")), 0);

    query = String::create(" ");
    obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create(
        "{\" \":\"\"}")), 0);

    query = String::create("&");
    obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create(
        "{\"\":[\"\",\"\"]}")), 0);

    query = String::create("=");
    obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create(
        "{\"\":\"\"}")), 0);

    query = String::create("=x&y&");
    obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create(
        "{\"\":[\"x\",\"\"],\"y\":\"\"}")), 0);

    query = String::create("foo=bar&baz=qux&baz=quux&corge");
    obj = querystring::parse(query);
    ASSERT_EQ(json::stringify(obj)->compareTo(String::create(
        "{\"baz\":[\"qux\",\"quux\"],\"corge\":\"\",\"foo\":\"bar\"}")), 0);
}

TEST(GTestQueryString, TestStringify) {
    JsObject::Ptr obj = JsObject::create();
    String::CPtr query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create()), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), String::create("b"));
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("a=b")), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), String::create("b"));
    obj->put(String::create("x"), 1);
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("a=b&x=1")), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), JsObject::create());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("a=")), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), JsArray::create());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create()), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), Null::instance());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("a=")), 0);

    obj = JsObject::create();
    obj->put(String::create("a"), Undefined::instance());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("a=")), 0);

    obj = JsObject::create();
    obj->put(String::create(), String::create());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("=")), 0);

    obj = JsObject::create();
    obj->put(Null::instance(), Null::instance());
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(String::create("null=")), 0);

    obj = JsObject::create();
    JsArray::Ptr ary = JsArray::create();
    ary->add(String::create("xyz"));
    ary->add(123);
    obj->put(Undefined::instance(), ary);
    query = querystring::stringify(obj);
    ASSERT_EQ(query->compareTo(
        String::create("undefined=xyz&undefined=123")), 0);
}

}  // namespace node
}  // namespace libj"
