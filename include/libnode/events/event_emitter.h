// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_EVENTS_EVENT_EMITTER_H_

#include <libj/js_array.h>
#include <libj/js_function.h>
#include <libj/js_object.h>
#include <libj/symbol.h>

namespace libj {
namespace node {
namespace events {

class EventEmitter : LIBJ_JS_OBJECT(EventEmitter)
 public:
    static Symbol::CPtr EVENT_NEW_LISTENER;

    static Ptr create();

    virtual void on(
        String::CPtr event,
        JsFunction::Ptr listener) = 0;

    virtual void once(
        String::CPtr event,
        JsFunction::Ptr listener) = 0;

    virtual void addListener(
        String::CPtr event,
        JsFunction::Ptr listener) = 0;

    virtual void removeListener(
        String::CPtr event,
        JsFunction::CPtr listener) = 0;

    virtual void removeAllListeners() = 0;

    virtual void removeAllListeners(String::CPtr event) = 0;

    virtual JsArray::Ptr listeners(String::CPtr event) = 0;

    virtual void emit(
        String::CPtr event,
        JsArray::Ptr args = JsArray::null()) = 0;

 public:
    void emit(
        String::CPtr event,
        const Value& v1);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8);

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8, const Value& v9);
};

}  // namespace events
}  // namespace node
}  // namespace libj

#include <libnode/impl/events/event_emitter.h>

#define LIBNODE_EVENT_EMITTER(T) \
    public libj::node::events::EventEmitter { \
    LIBJ_MUTABLE_DEFS(T, libj::node::events::EventEmitter)

#endif  // LIBNODE_EVENTS_EVENT_EMITTER_H_
