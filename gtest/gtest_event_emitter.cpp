// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/events/event_emitter.h>

#include <libj/error.h>
#include <libj/debug_print.h>

namespace libj {
namespace node {
namespace events {

static JsArray::Ptr results() {
    static JsArray::Ptr rs = JsArray::null();
    if (!rs) {
        rs = JsArray::create();
        LIBJ_DEBUG_PRINT(
            "static: JsArray %p",
            LIBJ_DEBUG_OBJECT_PTR(rs));
    }
    return rs;
}

class Add : LIBJ_JS_FUNCTION(Add)
 public:
    Value operator()(JsArray::Ptr args) {
        if (!args) {
            Value err = Error::create(Error::ILLEGAL_ARGUMENT);
            results()->add(err);
            return err;
        }

        Int sum = 0;
        for (Size i = 0; i < args->length(); i++) {
            int x;
            if (to<Int>(args->get(i), &x)) {
                sum += x;
            } else {
                Value err = Error::create(Error::ILLEGAL_ARGUMENT);
                results()->add(err);
                return err;
            }
        }
        results()->add(sum);
        return sum;
    }

    static Add::Ptr create() {
        return Ptr(new Add());
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
            results()->add(res);
            return res;
        } else {
            Value res = Error::create(Error::ILLEGAL_ARGUMENT);
            results()->add(res);
            return res;
        }
    }

    static Sub::Ptr create() {
        return Ptr(new Sub());
    }
};

TEST(GTestEventEmitter, TestCreate) {
    EventEmitter::Ptr ee = EventEmitter::create();
    ASSERT_TRUE(!!ee);
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

    results()->clear();
    JsArray::Ptr args = JsArray::create();
    args->add(5);
    args->add(3);
    ee->emit(event, args);

    ASSERT_TRUE(results()->get(0).equals(8));
    ASSERT_TRUE(results()->get(1).equals(2));

    ee->emit(event);
    ASSERT_TRUE(results()->get(2).instanceof(Type<Error>::id()));
    ASSERT_TRUE(results()->get(3).instanceof(Type<Error>::id()));

    ee->emit(event, static_cast<Value>(args));
    ASSERT_TRUE(results()->get(4).instanceof(Type<Error>::id()));
    ASSERT_TRUE(results()->get(5).instanceof(Type<Error>::id()));
}

TEST(GTestEventEmitter, TestEmit2) {
    EventEmitter::Ptr ee = EventEmitter::create();
    String::CPtr event = String::create("event");
    JsFunction::Ptr add = Add::create();
    ee->on(event, add);

    results()->clear();
    ee->emit(event, 1);
    ee->emit(event, 1, 2);
    ee->emit(event, 1, 2, 3);
    ee->emit(event, 1, 2, 3, 4);
    ee->emit(event, 1, 2, 3, 4, 5);
    ee->emit(event, 1, 2, 3, 4, 5, 6);
    ee->emit(event, 1, 2, 3, 4, 5, 6, 7);
    ee->emit(event, 1, 2, 3, 4, 5, 6, 7, 8);
    ee->emit(event, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    ASSERT_TRUE(results()->get(0).equals(1));
    ASSERT_TRUE(results()->get(1).equals(3));
    ASSERT_TRUE(results()->get(2).equals(6));
    ASSERT_TRUE(results()->get(3).equals(10));
    ASSERT_TRUE(results()->get(4).equals(15));
    ASSERT_TRUE(results()->get(5).equals(21));
    ASSERT_TRUE(results()->get(6).equals(28));
    ASSERT_TRUE(results()->get(7).equals(36));
    ASSERT_TRUE(results()->get(8).equals(45));
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

    results()->clear();
    JsArray::Ptr args = JsArray::create();
    args->add(6);
    args->add(3);
    ee->emit(event, args);

    ASSERT_EQ(1, ee->listeners(event)->length());
    Value v = ee->listeners(event)->get(0);
    JsFunction::Ptr f = toPtr<JsFunction>(v);
    ASSERT_EQ(sub, f);

    Value v0 = results()->get(0);
    Value v1 = results()->get(1);
    Int i0, i1;
    to<Int>(v0, &i0);
    to<Int>(v1, &i1);
    ASSERT_EQ(9, i0);
    ASSERT_EQ(3, i1);
}

}  // namespace events
}  // namespace node
}  // namespace libj
