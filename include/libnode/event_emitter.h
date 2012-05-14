// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_EVENT_EMITTER_H_
#define LIBNODE_EVENT_EMITTER_H_

#include "libj/string.h"
#include "libnode/js_array.h"
#include "libnode/js_function.h"
#include "libnode/js_object.h"

namespace libj {
namespace node {
namespace events {

class EventEmitter : LIBNODE_JS_OBJECT(EventEmitter)
 public:
    virtual void on(String::CPtr event, JsFunction::Ptr listener) = 0;
    virtual void addListener(String::CPtr event, JsFunction::Ptr listener) = 0;
    virtual void removeListener(String::CPtr event, JsFunction::CPtr listener) = 0;
    virtual void removeAllListeners() = 0;
    virtual void removeAllListeners(String::CPtr event) = 0;
    virtual void emit(String::CPtr event, JsArray::CPtr args) = 0;
    virtual Value listeners(String::CPtr event) = 0;
};

#define LIBNODE_EVENT_EMITTER(T) public libj::node::events::EventEmitter { \
    LIBJ_MUTABLE_DECLS(T, libj::node::events::EventEmitter)

#define LIBNODE_EVENT_EMITTER_WITHOUT_CREATE(T) public libj::node::events::EventEmitter { \
    LIBJ_MUTABLE_DECLS_WITHOUT_CREATE(T, libj::node::events::EventEmitter)

#define LIBNODE_EVENT_EMITTER_IMPL(EE) \
    LIBNODE_JS_OBJECT_IMPL(EE); \
 public: \
    void on(String::CPtr event, JsFunction::Ptr listener) { \
        EE->on(event, listener); \
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
    void emit(String::CPtr event, JsArray::CPtr args) { \
        EE->emit(event, args); \
    } \
    Value listeners(String::CPtr event) { \
        return EE->listeners(event); \
    }

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENT_EMITTER_H_