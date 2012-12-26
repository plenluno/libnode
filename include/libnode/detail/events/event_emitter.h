// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_

#include <libj/js_object.h>
#include <libj/js_function.h>
#include <libj/status.h>
#include <libj/detail/js_object.h>
#include <libnode/detail/flags.h>

namespace libj {
namespace node {
namespace detail {
namespace events {

template<typename I>
class EventEmitter
    : public libj::detail::JsObject<I>
    , public node::detail::Flags {
 public:
    EventEmitter() : events_(JsObject::create()) {}

    virtual void on(String::CPtr event, JsFunction::Ptr listener) {
        addListener(event, listener);
    }

    virtual void once(String::CPtr event, JsFunction::Ptr listener) {
        if (!listener) return;

        JsFunction::Ptr cb(new Once(event, listener, this));
        addListener(event, cb);
    }

    virtual void addListener(
        String::CPtr event, JsFunction::Ptr listener) {
        if (!listener) return;

        JsArray::Ptr a;
        Value v = events_->get(event);
        if (v.isUndefined()) {
            a = JsArray::create();
            events_->put(String::intern(event), a);
        } else {
            a = toPtr<JsArray>(v);
        }
        a->add(listener);
        emit(I::EVENT_NEW_LISTENER);
    }

    virtual void removeListener(
        String::CPtr event, JsFunction::CPtr listener) {
        JsArray::Ptr a = listeners(event);
        Size n = a->size();
        for (Size i = 0; i < n; i++) {
            Value v = a->get(i);
            if (v.instanceof(Type<Once>::id())) {
                typename Once::Ptr once = toPtr<Once>(v);
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

    virtual void removeAllListeners() {
        events_->clear();
    }

    virtual void removeAllListeners(String::CPtr event) {
        events_->remove(event);
    }

    virtual void emit(
        String::CPtr event, JsArray::Ptr args = JsArray::null()) {
        JsArray::Ptr a = listeners(event);
        Size i = 0;
        while (i < a->size()) {
            Value v = a->get(i);
            if (!v.instanceof(Type<Once>::id())) i++;
            JsFunction::Ptr f = toPtr<JsFunction>(v);
            (*f)(args);
        }
    }

    virtual JsArray::Ptr listeners(String::CPtr event) {
        Value v = events_->get(event);
        if (v.isUndefined()) {
            JsArray::Ptr a = JsArray::create();
            events_->put(event, a);
            return a;
        } else {
            return toPtr<JsArray>(v);
        }
    }

 private:
    class Once : LIBJ_JS_FUNCTION_TEMPLATE(Once)
     public:
        Once(
            String::CPtr event,
            JsFunction::Ptr listener,
            EventEmitter* ee)
            : event_(event)
            , listener_(listener)
            , ee_(ee) {}

        Value operator()(JsArray::Ptr args) {
            (*listener_)(args);
            ee_->removeListener(event_, listener_);
            return libj::Status::OK;
        }

        JsFunction::Ptr listener() {
            return listener_;
        }

     private:
        String::CPtr event_;
        JsFunction::Ptr listener_;
        EventEmitter* ee_;
    };

 private:
    JsObject::Ptr events_;
};

}  // namespace events
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
