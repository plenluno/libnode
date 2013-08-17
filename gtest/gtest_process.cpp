// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/process.h>

#include <libj/status.h>

namespace libj {
namespace node {

class GTestProcessNextTick1 : LIBJ_JS_FUNCTION(GTestProcessNextTick1)
 public:
    GTestProcessNextTick1(JsArray::Ptr a) : a_(a) {}

    virtual Value operator()(JsArray::Ptr args) {
        a_->push(1);
        return Status::OK;
    }

 private:
    JsArray::Ptr a_;
};

class GTestProcessNextTick2 : LIBJ_JS_FUNCTION(GTestProcessNextTick2)
 public:
    GTestProcessNextTick2(JsArray::Ptr a) : a_(a) {}

    virtual Value operator()(JsArray::Ptr args) {
        a_->push(2);
        return Status::OK;
    }

 private:
    JsArray::Ptr a_;
};

class GTestProcessNextTick3 : LIBJ_JS_FUNCTION(GTestProcessNextTick3)
 public:
    GTestProcessNextTick3(JsArray::Ptr a) : a_(a) {}

    virtual Value operator()(JsArray::Ptr args) {
        a_->push(3);
        process::nextTick(JsFunction::Ptr(new GTestProcessNextTick2(a_)));
        return Status::OK;
    }

 private:
    JsArray::Ptr a_;
};

TEST(GTestProcess, TestNextTick) {
    JsArray::Ptr a = JsArray::create();
    process::nextTick(JsFunction::Ptr(new GTestProcessNextTick1(a)));
    process::nextTick(JsFunction::Ptr(new GTestProcessNextTick3(a)));
    node::run();
    ASSERT_TRUE(a->toString()->equals(str("1,3,2")));
}

}  // namespace node
}  // namespace libj
