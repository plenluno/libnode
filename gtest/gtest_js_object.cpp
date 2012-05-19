// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/js_object.h>

namespace libj {
namespace node {

class XObj : LIBNODE_JS_OBJECT(XObj)
};

class XObjImpl : public XObj {
 public:
    static Ptr create() {
        Ptr p(new XObjImpl());
        return p;
    }
    
 private:
    JsObject::Ptr jo_;
    
    XObjImpl()
        : jo_(JsObject::create()) {}
    
    LIBNODE_JS_OBJECT_IMPL(jo_);
};

XObj::Ptr XObj::create() {
    return XObjImpl::create();
}


TEST(GTestObject, TestGetCPtr) {
    JsObject::Ptr obj = JsObject::create();
    String::CPtr abc = String::create("abc");
    obj->put(abc, abc);
    ASSERT_EQ(obj->getCPtr<String>(abc)->compareTo(abc), 0);
    
    XObj::Ptr xobj = XObj::create();
    xobj->put(abc, abc);
    ASSERT_EQ(xobj->getCPtr<String>(abc)->compareTo(abc), 0);
}

}  // namespace node
}  // namespace libj