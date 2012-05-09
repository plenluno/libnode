// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace events {

class EventEmitterImpl : public EventEmitter {
 public:
    void on(String::CPtr event, JsFunction::Ptr listener) {
        addListener(event, listener);
    }
    
    void addListener(String::CPtr event, JsFunction::Ptr listener) {
        JsArray::Ptr a;
        Value v = listeners_->get(event);
        if (v.instanceOf(Type<Null>::id())) {
            a = JsArray::create();
            listeners_->put(event, a);
        } else {
            a = toPtr<JsArray>(v);
        }
        a->add(listener);
    }
    
    void removeListener(String::CPtr event, JsFunction::CPtr listener) {
        Value v = listeners_->get(event);
        if (!v.instanceOf(Type<Null>::id())) {
            JsArray::Ptr a = toPtr<JsArray>(v);
            while (a->remove(listener)) {}
        }
    }
    
    void removeAllListeners() {
        listeners_->clear();
    }
    
    void removeAllListeners(String::CPtr event) {
        Value v = listeners_->get(event);
        if (!v.instanceOf(Type<Null>::id())) {
            JsArray::Ptr a = toPtr<JsArray>(v);
            a->clear();
        }
    }
    
    void emit(String::CPtr event, JsArray::CPtr args) {
        Value v = listeners_->get(event);
        if (!v.instanceOf(Type<Null>::id())) {
            JsArray::Ptr a = toPtr<JsArray>(v);
            Size n = a->size();
            for (Size i = 0; i < n; i++) {
                JsFunction::Ptr f = toPtr<JsFunction>(a->get(i));
                (*f)(args);
            }
        }
    }
    
    Value listeners(String::CPtr event) {
        return listeners_->get(event);
    }

    static Ptr create() {
        Ptr p(new EventEmitterImpl());
        return p;
    }

 private:
     JsObject::Ptr listeners_;
    
    EventEmitterImpl()
        : listeners_(JsObject::create()) {}
    
    LIBNODE_JS_OBJECT_IMPL(listeners_);
};

EventEmitter::Ptr EventEmitter::create() {
    return EventEmitterImpl::create();
}

}  // namespace events
}  // namespace node
}  // namespace libj
