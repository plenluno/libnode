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
    virtual void emit(String::CPtr event, JsArray::Ptr args) = 0;
    virtual JsArray::Ptr listeners(String::CPtr event) = 0;
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
    void emit(String::CPtr event, JsArray::Ptr args) { \
        EE->emit(event, args); \
    } \
    JsArray::Ptr listeners(String::CPtr event) { \
        return EE->listeners(event); \
    }

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENTS_EVENT_EMITTER_H_
