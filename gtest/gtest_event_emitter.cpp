// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/event_emitter.h>
#include <libj/error.h>

namespace libj {
namespace node {

static Type<JsArray>::Ptr results = JsArray::create();

class Add : LIBNODE_JS_FUNCTION(Add)
};

class AddImpl : public Add {
 public:
    Value operator()(Type<JsArray>::Cptr args) {
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

    static Type<Add>::Ptr create() {
        Type<Add>::Ptr p(new AddImpl());
        return p;
    }
};

Type<Add>::Ptr Add::create() {
    return AddImpl::create();
}

class Sub : LIBNODE_JS_FUNCTION(Sub)
};

class SubImpl : public Sub {
 public:
    Value operator()(Type<JsArray>::Cptr args) {
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

    static Type<Sub>::Ptr create() {
        Type<Sub>::Ptr p(new SubImpl());
        return p;
    }
};

Type<Sub>::Ptr Sub::create() {
    return SubImpl::create();
}


TEST(GTestEventEmitter, TestCreate) {
    Type<EventEmitter>::Ptr ee = EventEmitter::create();
    ASSERT_TRUE(ee ? true : false);
}

TEST(GTestEventEmitter, TestAddAndGetListeners) {
    Type<EventEmitter>::Ptr ee = EventEmitter::create();
    Type<String>::Cptr event = String::create("event");
    Type<JsFunction>::Ptr add = Add::create();
    Type<JsFunction>::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->addListener(String::create("event"), sub);
    
    Value v = ee->listeners(event);
    ASSERT_TRUE(v.instanceOf(Type<JsArray>::id()));
    
    Type<JsArray>::Ptr a;
    toPtr<JsArray>(v, &a);
    ASSERT_EQ(a->size(), 2);
    
    v = a->get(0);
    Type<JsFunction>::Ptr f1;
    toPtr<JsFunction>(v, &f1);
    ASSERT_TRUE(f1 == add || f1 == sub);
    v = a->get(1);
    Type<JsFunction>::Ptr f2;
    toPtr<JsFunction>(v, &f2);
    ASSERT_TRUE(f1 != f2 && (f1 == add || f1 == sub));
}

TEST(GTestEventEmitter, TestEmit) {
    Type<EventEmitter>::Ptr ee = EventEmitter::create();
    Type<String>::Cptr event = String::create("event");
    Type<JsFunction>::Ptr add = Add::create();
    Type<JsFunction>::Ptr sub = Sub::create();
    ee->on(event, add);
    ee->on(event, sub);
    
    Type<JsArray>::Ptr args = JsArray::create();
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

}  // namespace node
}  // namespace libj
