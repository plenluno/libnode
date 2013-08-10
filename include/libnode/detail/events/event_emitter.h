// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_

#include <libnode/debug_print.h>
#include <libnode/detail/flags.h>

#include <libj/status.h>
#include <libj/js_function.h>
#include <libj/js_object.h>
#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace detail {
namespace events {

template<typename I>
class EventEmitter
    : public libj::detail::JsObject<I>
    , public node::detail::Flags {
 public:
    EventEmitter()
        : maxListeners_(10)
        , events_(libj::JsObject::create()) {}

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

        JsArray::Ptr ls = listeners(event);
        ls->add(listener);
        if (maxListeners_ && ls->length() > maxListeners_) {
            LIBNODE_DEBUG_PRINT("%d listeners added", ls->length());
        }
        emit(I::EVENT_NEW_LISTENER);
    }

    virtual void removeListener(
        String::CPtr event, JsFunction::CPtr listener) {
        JsArray::Ptr a = listeners(event);
        Size n = a->size();
        for (Size i = 0; i < n; i++) {
            Value v = a->get(i);
            if (v.is<Once>()) {
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

    virtual void setMaxListeners(Size max) {
        maxListeners_ = max;
    }

    virtual JsArray::Ptr listeners(String::CPtr event) {
        JsArray::Ptr a = events_->getPtr<JsArray>(event);
        if (a) {
            return a;
        } else {
            a = JsArray::create();
            events_->put(String::intern(event), a);
            return a;
        }
    }

    virtual Boolean emit(
        String::CPtr event, JsArray::Ptr args = JsArray::null()) {
        JsArray::Ptr a = listeners(event);
        if (a->isEmpty()) return false;

        Size i = 0;
        while (i < a->size()) {
            Value v = a->get(i);
            if (!v.is<Once>()) i++;
            JsFunction::Ptr f = toPtr<JsFunction>(v);
            (*f)(args);
        }
        return true;
    }

 public:
    virtual Boolean emit(
        String::CPtr event,
        const Value& v1) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        return emit(event, args);
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        return emit(event, args);
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        return emit(event, args);
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        return emit(event, args);
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5) {
        JsArray::Ptr args = JsArray::create();
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        return emit(event, args);
    }

    virtual Boolean emit(
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
        return emit(event, args);
    }

    virtual Boolean emit(
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
        return emit(event, args);
    }

    virtual Boolean emit(
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
        return emit(event, args);
    }

    virtual Boolean emit(
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
        return emit(event, args);
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
            return Status::OK;
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
    Size maxListeners_;
    libj::JsObject::Ptr events_;
};

}  // namespace events
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
