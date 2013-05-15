// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_

#include <libnode/config.h>
#include <libnode/detail/flags.h>

#include <libj/debug_print.h>
#include <libj/js_function.h>
#include <libj/js_object.h>
#include <libj/status.h>
#include <libj/detail/js_object.h>

#ifdef LIBNODE_USE_SP
# ifdef LIBNODE_DEBUG
#  define LIBNODE_ARGS_CREATE(A) \
    static JsArray::Ptr A = JsArray::null(); \
    if (!A) { \
        A = JsArray::create(); \
        LIBJ_DEBUG_PRINT( \
            "static: args@EventEmitter::emit %p", \
            LIBJ_DEBUG_OBJECT_PTR(A)); \
    }
#  define LIBNODE_ARGS_CLEAR(A) \
    if (A.use_count() == 1) { \
        A->clear(); \
    } else { \
        A = JsArray::create(); \
        LIBJ_DEBUG_PRINT( \
            "static: args@EventEmitter::emit %p", \
            LIBJ_DEBUG_OBJECT_PTR(A)); \
    }
# else
#  define LIBNODE_ARGS_CREATE(A) \
    static JsArray::Ptr A = JsArray::create();
#  define LIBNODE_ARGS_CLEAR(A) \
    if (A.use_count() == 1) { \
        A->clear(); \
    } else { \
        A = JsArray::create(); \
    }
# endif
#else
# define LIBNODE_ARGS_CREATE(A) \
    JsArray::Ptr A = JsArray::create();
# define LIBNODE_ARGS_CLEAR(A)
#endif

namespace libj {
namespace node {
namespace detail {
namespace events {

template<typename I>
class EventEmitter
    : public libj::detail::JsObject<I>
    , public node::detail::Flags {
 public:
    EventEmitter() : events_(libj::JsObject::create()) {}

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

        listeners(event)->add(listener);
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
            if (!v.instanceof(Type<Once>::id())) i++;
            JsFunction::Ptr f = toPtr<JsFunction>(v);
            (*f)(args);
        }
        return true;
    }

 public:
    virtual Boolean emit(
        String::CPtr event,
        const Value& v1) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        args->add(v8);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
    }

    virtual Boolean emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8, const Value& v9) {
        LIBNODE_ARGS_CREATE(args);
        args->add(v1);
        args->add(v2);
        args->add(v3);
        args->add(v4);
        args->add(v5);
        args->add(v6);
        args->add(v7);
        args->add(v8);
        args->add(v9);
        Boolean r = emit(event, args);
        LIBNODE_ARGS_CLEAR(args);
        return r;
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
    libj::JsObject::Ptr events_;
};

}  // namespace events
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_EVENTS_EVENT_EMITTER_H_
