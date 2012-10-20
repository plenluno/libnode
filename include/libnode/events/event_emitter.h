// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_EVENTS_EVENT_EMITTER_H_

#include <libj/js_array.h>
#include <libj/js_function.h>
#include <libj/js_object.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace events {

class EventEmitter : LIBJ_JS_OBJECT(EventEmitter)
 public:
    static const String::CPtr EVENT_NEW_LISTENER;

    static Ptr create();

    virtual void on(
        String::CPtr event, JsFunction::Ptr listener) = 0;
    virtual void once(
        String::CPtr event, JsFunction::Ptr listener) = 0;
    virtual void addListener(
        String::CPtr event, JsFunction::Ptr listener) = 0;
    virtual void removeListener(
        String::CPtr event, JsFunction::CPtr listener) = 0;
    virtual void removeAllListeners() = 0;
    virtual void removeAllListeners(String::CPtr event) = 0;
    virtual JsArray::Ptr listeners(String::CPtr event) = 0;
    virtual void emit(
        String::CPtr event, JsArray::Ptr args = JsArray::null()) = 0;

 public:
    void emit(
        String::CPtr event,
        const Value& val) {
        JsArray::Ptr args = JsArray::create();
        args->add(val);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        args->add(v8);
        emit(event, args);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8, const Value& v9) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        args->add(v8);
        args->add(v9);
        emit(event, args);
    }
};

#define LIBNODE_EVENT_EMITTER(T) \
    public libj::node::events::EventEmitter { \
    LIBJ_MUTABLE_DEFS(T, libj::node::events::EventEmitter)

#define LIBNODE_EVENT_EMITTER_IMPL(EE) \
    LIBJ_JS_OBJECT_IMPL(EE); \
public: \
    void on(String::CPtr event, JsFunction::Ptr listener) { \
        EE->on(event, listener); \
    } \
    void once(String::CPtr event, JsFunction::Ptr listener) { \
        EE->once(event, listener); \
    } \
    void addListener(String::CPtr event, JsFunction::Ptr listener) { \
        EE->addListener(event, listener); \
    } \
    void removeListener(String::CPtr event, JsFunction::CPtr listener) { \
        EE->removeListener(event, listener); \
    } \
    void removeAllListeners() { \
        EE->removeAllListeners(); \
    } \
    void removeAllListeners(String::CPtr event) { \
        EE->removeAllListeners(event); \
    } \
    JsArray::Ptr listeners(String::CPtr event) { \
        return EE->listeners(event); \
    } \
    void emit(String::CPtr event, JsArray::Ptr args = JsArray::null()) { \
        EE->emit(event, args); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1) { \
        EE->emit(event, v1); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2) { \
        EE->emit(event, v1, v2); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3) { \
        EE->emit(event, v1, v2, v3); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4) { \
        EE->emit(event, v1, v2, v3, v4); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5) { \
        EE->emit(event, v1, v2, v3, v4, v5); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7, const Value& v8) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7, v8); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7, const Value& v8, const Value& v9) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7, v8, v9); \
    }

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENTS_EVENT_EMITTER_H_
