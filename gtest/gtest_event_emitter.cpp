// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/events/event_emitter.h>
#include <libj/error.h>

namespace libj {
namespace node {
namespace events {

static JsArray::Ptr results = JsArray::create();

class Add : LIBJ_JS_FUNCTION(Add)
 public:
    Value operator()(JsArray::Ptr args) {
        if (args &&
            args->size() == 2 &&
            args->get(0).type() == Type<Int>::id() &&
            args->get(1).type() == Type<Int>::id()) {
            int x, y;
            to<Int>(args->get(0), &x);
            to<Int>(args->get(1), &y);
            Value res = x + y;
            results->add(res);
            return res;
        } else {
            Value res = Error::create(Error::ILLEGAL_ARGUMENT);
            results->add(res);
            return res;
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
        if (args &&
            args->size() == 2 &&
            args->get(0).type() == Type<Int>::id() &&
            args->get(1).type() == Type<Int>::id()) {
            int x, y;
            to<Int>(args->get(0), &x);
            to<Int>(args->get(1), &y);
            Value res = x - y;
            results->add(res);
            return res;
        } else {
            Value res = Error::create(Error::ILLEGAL_ARGUMENT);
            results->add(res);
            return res;
        }
    }

    static Sub::Ptr create() {
        Sub::Ptr p(new Sub());
        return p;
    }
};

TEST(GTestEventEmitter, TestCreate) {
    EventEmitter::Ptr ee = EventEmitter::create();
    ASSERT_TRUE(ee);
}

TEST(GTestEventEmitter, TestOnAndAddListener) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->addListener(String::create("event"), sub);

    JsArray::Ptr a = ee->listeners(event);
    ASSERT_EQ(2, a->size());

    Value v = a->get(0);
    JsFunction::Ptr f1 = toPtr<JsFunction>(v);
    ASSERT_EQ(add, f1);
    v = a->get(1);
    JsFunction::Ptr f2 = toPtr<JsFunction>(v);
    ASSERT_EQ(sub, f2);
}

TEST(GTestEventEmitter, TestEmit) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->on(event, sub);

    results->clear();
    JsArray::Ptr args = JsArray::create();
    args->add(5);
    args->add(3);
    ee->emit(event, args);

    Value v0 = results->get(0);
    Value v1 = results->get(1);
    Int i0, i1;
    to<Int>(v0, &i0);
    to<Int>(v1, &i1);
    ASSERT_EQ(8, i0);
    ASSERT_EQ(2, i1);

    ee->emit(event);
    results->get(2).instanceof(Type<Error>::id());
    results->get(3).instanceof(Type<Error>::id());
}

TEST(GTestEventEmitter, TestRemoveListener) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->once(event, add);
    ee->on(event, sub);
    ASSERT_EQ(2, ee->listeners(event)->length());

    ee->removeListener(event, add);

    ASSERT_EQ(1, ee->listeners(event)->length());
    Value v = ee->listeners(event)->get(0);
    JsFunction::Ptr f = toPtr<JsFunction>(v);
    ASSERT_EQ(sub, f);

    ee->removeListener(event, sub);
    ASSERT_TRUE(ee->listeners(event)->isEmpty());
}

TEST(GTestEventEmitter, TestRemoveAllListener) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->once(event, add);
    ee->on(event, sub);
    ASSERT_EQ(2, ee->listeners(event)->length());

    ee->removeAllListeners(event);
    ASSERT_TRUE(ee->listeners(event)->isEmpty());
}

TEST(GTestEventEmitter, TestOnce) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    JsFunction::Ptr sub = Sub::create();
    ee->once(event, add);
    ee->on(event, sub);
    ASSERT_EQ(2, ee->listeners(event)->length());

    results->clear();
    JsArray::Ptr args = JsArray::create();
    args->add(6);
    args->add(3);
    ee->emit(event, args);

    ASSERT_EQ(1, ee->listeners(event)->length());
    Value v = ee->listeners(event)->get(0);
    JsFunction::Ptr f = toPtr<JsFunction>(v);
    ASSERT_EQ(sub, f);

    Value v0 = results->get(0);
    Value v1 = results->get(1);
    Int i0, i1;
    to<Int>(v0, &i0);
    to<Int>(v1, &i1);
    ASSERT_EQ(9, i0);
    ASSERT_EQ(3, i1);
}

}  // namespace events
}  // namespace node
}  // namespace libj
