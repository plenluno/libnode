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
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{}")));

    query = String::create(" ");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{\" \":\"\"}")));

    query = String::create("&");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{\"\":[\"\",\"\"]}")));

    query = String::create("=");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{\"\":\"\"}")));

    query = String::create("=x&y&");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{\"\":[\"x\",\"\"],\"y\":\"\"}")));

    query = String::create("%20=%20");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(
        String::create("{\" \":\" \"}")));
}

TEST(GTestQueryString, TestStringify) {
    JsObject::Ptr obj = JsObject::create();
    String::CPtr query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create()));

    obj = JsObject::create();
    obj->put(String::create("a"), String::create("b"));
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("a=b")));

    obj = JsObject::create();
    obj->put(String::create("a"), String::create("b"));
    obj->put(String::create("x"), 1);
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("a=b&x=1")));

    obj = JsObject::create();
    obj->put(String::create("a"), JsObject::create());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("a=")));

    obj = JsObject::create();
    obj->put(String::create("a"), JsArray::create());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create()));

    obj = JsObject::create();
    obj->put(String::create("a"), Null::instance());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("a=")));

    obj = JsObject::create();
    obj->put(String::create("a"), Undefined::instance());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("a=")));

    obj = JsObject::create();
    obj->put(String::create(), String::create());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("=")));

    obj = JsObject::create();
    obj->put(Null::instance(), Null::instance());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("null=")));

    obj = JsObject::create();
    JsArray::Ptr ary = JsArray::create();
    ary->add(String::create("xyz"));
    ary->add(123);
    obj->put(Undefined::instance(), ary);
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(
        String::create("undefined=xyz&undefined=123")));

    obj = JsObject::create();
    obj->put(String::create(" "), String::create(" "));
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(String::create("%20=%20")));
}

}  // namespace node
}  // namespace libj"
