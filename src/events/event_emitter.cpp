// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/status.h>

#include "libnode/events/event_emitter.h"

namespace libj {
namespace node {
namespace events {

class EventEmitterImpl : public EventEmitter {
 private:
    class Once : LIBJ_JS_FUNCTION(Once)
     public:
        static Ptr create(
            String::CPtr event,
            JsFunction::Ptr listener,
            EventEmitterImpl* ee) {
            Ptr p(new Once(event, listener, ee));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            (*listener_)(args);
            ee_->removeListener(event_, listener_);
            return Status::OK;
        }

        JsFunction::Ptr listener() {
            return listener_;
        }

     private:
        String::CPtr event_;
        JsFunction::Ptr listener_;
        EventEmitterImpl* ee_;

        Once(
            String::CPtr event,
            JsFunction::Ptr listener,
            EventEmitterImpl* ee)
            : event_(event)
            , listener_(listener)
            , ee_(ee) {}
    };

 public:
    void on(String::CPtr event, JsFunction::Ptr listener) {
        addListener(event, listener);
    }

    void once(String::CPtr event, JsFunction::Ptr listener) {
        addListener(event, Once::create(event, listener, this));
    }

    void addListener(String::CPtr event, JsFunction::Ptr listener) {
        JsArray::Ptr a;
        Value v = listeners_->get(event);
        if (v.isEmpty()) {
            a = JsArray::create();
            listeners_->put(event, a);
        } else {
            a = toPtr<JsArray>(v);
        }
        a->add(listener);
        emit(EVENT_NEW_LISTENER);
    }

    void removeListener(String::CPtr event, JsFunction::CPtr listener) {
        JsArray::Ptr a = listeners(event);
        Size n = a->size();
        for (Size i = 0; i < n; i++) {
            Value v = a->get(i);
            if (v.instanceof(Type<Once>::id())) {
                Once::Ptr once = toPtr<Once>(v);
                if (once->listener()->equals(listener)) {
                    a->remove(i);
                    break;
                }
            } else {
                JsFunction::Ptr func = toPtr<JsFunction>(v);
                if (func->equals(listener)) {
                    a->remove(i);
                    break;
                }
            }
        }
    }

    void removeAllListeners() {
        listeners_->clear();
    }

    void removeAllListeners(String::CPtr event) {
        listeners_->remove(event);
    }

    void emit(String::CPtr event, JsArray::Ptr args = JsArray::null()) {
        JsArray::Ptr a = listeners(event);
        Size i = 0;
        while (i < a->size()) {
            Value v = a->get(i);
            if (!v.instanceof(Type<Once>::id())) i++;
            JsFunction::Ptr f = toPtr<JsFunction>(v);
            (*f)(args);
        }
    }

    JsArray::Ptr listeners(String::CPtr event) {
        Value v = listeners_->get(event);
        if (v.isEmpty()) {
            JsArray::Ptr a = JsArray::create();
            listeners_->put(event, a);
            return a;
        } else {
            return toPtr<JsArray>(v);
        }
    }

    static Ptr create() {
        Ptr p(new EventEmitterImpl());
        return p;
    }

 private:
    JsObject::Ptr listeners_;

    EventEmitterImpl()
        : listeners_(JsObject::create()) {}

    LIBJ_JS_OBJECT_IMPL(listeners_);
};

const String::CPtr EventEmitter::EVENT_NEW_LISTENER =
    String::create("newListener");

EventEmitter::Ptr EventEmitter::create() {
    return EventEmitterImpl::create();
}

}  // namespace events
}  // namespace node
}  // namespace libj
