// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_EVENT_EMITTER_H_
#define LIBNODE_EVENT_EMITTER_H_

#include "libj/string.h"
#include "libj/function.h"
#include "libnode/typedef.h"

namespace libj {
namespace node {
namespace events {

class EventEmitter : LIBNODE_JS_OBJECT(EventEmitter)
 public:
    virtual void on(Type<String>::Cptr event, Type<JsFunction>::Ptr listener) = 0;
    virtual void addListener(Type<String>::Cptr event, Type<JsFunction>::Ptr listener) = 0;
    virtual void removeListener(Type<String>::Cptr event, Type<JsFunction>::Cptr listener) = 0;
    virtual void removeAllListeners() = 0;
    virtual void removeAllListeners(Type<String>::Cptr event) = 0;
    virtual void emit(Type<String>::Cptr event, Type<JsArray>::Cptr args) = 0;
    virtual Value listeners(Type<String>::Cptr event) = 0;
};

#define LIBNODE_EVENT_EMITTER(T) public libj::node::events::EventEmitter { \
    LIBJ_MUTABLE_DECLS(T, libj::node::events::EventEmitter)

#define LIBNODE_EVENT_EMITTER_IMPL(EE) \
 public: \
    void on(Type<String>::Cptr event, Type<JsFunction>::Ptr listener) { \
        EE->on(event, listener); \
    } \
    void addListener(Type<String>::Cptr event, Type<JsFunction>::Ptr listener) { \
        EE->addListener(event, listener); \
    } \
    void removeListener(Type<String>::Cptr event, Type<JsFunction>::Cptr listener) { \
        EE->removeListener(event, listener); \
    } \
    void removeAllListeners() { \
        EE->removeAllListeners(); \
    } \
    void removeAllListeners(Type<String>::Cptr event) { \
        EE->removeAllListeners(event); \
    } \
    void emit(Type<String>::Cptr event, Type<JsArray>::Cptr args) { \
        EE->emit(event, args); \
    } \
    Value listeners(Type<String>::Cptr event) { \
        return EE->listeners(event); \
    }

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENT_EMITTER_H_