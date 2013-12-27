// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/querystring.h>

#include <libj/json.h>
#include <libj/js_array.h>

namespace libj {
namespace node {

TEST(GTestQueryString, TestParse) {
    String::CPtr query = str();
    JsObject::Ptr obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(str("{}")));

    query = str(" ");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(str("{\" \":\"\"}")));

    query = str("&");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(str("{\"\":[\"\",\"\"]}")));

    query = str("=");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(str("{\"\":\"\"}")));

    query = str("=x&y&");
    obj = querystring::parse(query);
    String::CPtr s1 = json::stringify(obj);
    String::CPtr s2 = str("{\"\":[\"x\",\"\"],\"y\":\"\"}");
    String::CPtr s3 = str("{\"y\":\"\",\"\":[\"x\",\"\"]}");
    ASSERT_TRUE(s1->equals(s2) || s1->equals(s3));

    query = str("%20=%20");
    obj = querystring::parse(query);
    ASSERT_TRUE(json::stringify(obj)->equals(str("{\" \":\" \"}")));
}

TEST(GTestQueryString, TestStringify) {
    JsObject::Ptr obj = JsObject::create();
    String::CPtr query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str()));

    obj = JsObject::create();
    obj->put(str("a"), str("b"));
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("a=b")));

    obj = JsObject::create();
    obj->put(str("a"), str("b"));
    obj->put(str("x"), 1);
    query = querystring::stringify(obj);
    ASSERT_TRUE(
        query->equals(str("a=b&x=1")) ||
        query->equals(str("x=1&a=b")));

    obj = JsObject::create();
    obj->put(str("a"), JsObject::create());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("a=")));

    obj = JsObject::create();
    obj->put(str("a"), JsArray::create());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str()));

    obj = JsObject::create();
    obj->put(str("a"), Object::null());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("a=")));

    obj = JsObject::create();
    obj->put(str("a"), UNDEFINED);
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("a=")));

    obj = JsObject::create();
    obj->put(str(), str());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("=")));

    obj = JsObject::create();
    obj->put(Object::null(), Object::null());
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("null=")));

    obj = JsObject::create();
    JsArray::Ptr ary = JsArray::create();
    ary->add(str("xyz"));
    ary->add(123);
    obj->put(UNDEFINED, ary);
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("undefined=xyz&undefined=123")));

    obj = JsObject::create();
    obj->put(str(" "), str(" "));
    query = querystring::stringify(obj);
    ASSERT_TRUE(query->equals(str("%20=%20")));
}

}  // namespace node
}  // namespace libj"
