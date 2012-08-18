// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/null.h>
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
            p->setSelf(p);
            return p;
        }

        Value operator()(ArrayList::Ptr args) {
            (*listener_)(args);
            Ptr self = unsetSelf();
            ee_->removeListener(event_, self);
            return Status::OK;
        }

     private:
        String::CPtr event_;
        JsFunction::Ptr listener_;
        EventEmitterImpl* ee_;
        Once::Ptr self_;

        Once(
            String::CPtr event,
            JsFunction::Ptr listener,
            EventEmitterImpl* ee)
            : event_(event)
            , listener_(listener)
            , ee_(ee) {}

        void setSelf(Once::Ptr self) {
            self_ = self;
        }

        Ptr unsetSelf() {
            Ptr nullp(LIBJ_NULL(Once));
            Ptr self;
            self = self_;
            self_ = nullp;
            return self;
        }
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
        if (v.instanceof(Type<Null>::id())) {
            a = JsArray::create();
            listeners_->put(event, a);
        } else {
            a = toPtr<JsArray>(v);
        }
        a->add(listener);
        emit(EVENT_NEW_LISTENER, JsArray::create());
    }

    void removeListener(String::CPtr event, JsFunction::CPtr listener) {
        JsArray::Ptr a = listeners(event);
        a->remove(listener);
    }

    void removeAllListeners() {
        listeners_->clear();
    }

    void removeAllListeners(String::CPtr event) {
        listeners_->remove(event);
    }

    void emit(String::CPtr event, JsArray::Ptr args) {
        JsArray::Ptr a = listeners(event);
        Size n = a->size();
        for (Size i = 0; i < n; i++) {
            JsFunction::Ptr f = toPtr<JsFunction>(a->get(i));
            (*f)(args);
        }
    }

    JsArray::Ptr listeners(String::CPtr event) {
        Value v = listeners_->get(event);
        if (v.instanceof(Type<Null>::id())) {
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
