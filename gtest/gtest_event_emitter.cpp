// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/event_emitter.h>
#include <libj/error.h>

namespace libj {
namespace node {
namespace events {

static JsArray::Ptr results = JsArray::create();

class Add : LIBJ_JS_FUNCTION(Add)
 public:
    Value operator()(JsArray::Ptr args) {
        if (args->size() == 2 &&
            args->get(0).type() == Type<Int>::id() &&
            args->get(1).type() == Type<Int>::id()) {
            int x, y;
            to<Int>(args->get(0), &x);
            to<Int>(args->get(1), &y);
            Value res = x + y;
            results->add(res);
            return res;
        } else {
            return Error::create(Error::ILLEGAL_ARGUMENT);
        }
    }

    static Add::Ptr create() {
        Add::Ptr p(new Add());
        return p;
    }
};

class Sub : LIBJ_JS_FUNCTION(Sub)
 public:
    Value operator()(JsArray::Ptr args) {
        if (args->size() == 2 &&
            args->get(0).type() == Type<Int>::id() &&
            args->get(1).type() == Type<Int>::id()) {
            int x, y;
            to<Int>(args->get(0), &x);
            to<Int>(args->get(1), &y);
            Value res = x - y;
            results->add(res);
            return res;
        } else {
            return Error::create(Error::ILLEGAL_ARGUMENT);
        }
    }

    static Sub::Ptr create() {
        Sub::Ptr p(new Sub());
        return p;
    }
};

TEST(GTestEventEmitter, TestCreate) {
    EventEmitter::Ptr ee = EventEmitter::create();
    ASSERT_TRUE(ee ? true : false);
}

TEST(GTestEventEmitter, TestAddAndGetListeners) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->addListener(String::create("event"), sub);

    Value v = ee->listeners(event);
    ASSERT_TRUE(v.instanceof(Type<JsArray>::id()));

    JsArray::Ptr a = toPtr<JsArray>(v);
    ASSERT_EQ(a->size(), 2);

    v = a->get(0);
    JsFunction::Ptr f1 = toPtr<JsFunction>(v);
    ASSERT_TRUE(f1 == add || f1 == sub);
    v = a->get(1);
    JsFunction::Ptr f2 = toPtr<JsFunction>(v);
    ASSERT_TRUE(f1 != f2 && (f1 == add || f1 == sub));
}

TEST(GTestEventEmitter, TestEmit) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->on(event, sub);

    JsArray::Ptr args = JsArray::create();
    args->add(5);
    args->add(3);
    ee->emit(event, args);

    Value v0 = results->get(0);
    Value v1 = results->get(1);
    Int i0, i1;
    to<Int>(v0, &i0);
    to<Int>(v1, &i1);
    ASSERT_TRUE(i0 == 2 || i0 == 8);
    ASSERT_TRUE(i0 != i1 && (i1 == 2 || i1 == 8));
}

}  // namespace events
}  // namespace node
}  // namespace libj
