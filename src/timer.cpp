// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/timer.h"

#include "./uv/timer.h"

namespace libj {
namespace node {

namespace {

    class OnTimeout : LIBJ_JS_FUNCTION(OnTimeout)
     public:
        static Ptr create(
            JsFunction::Ptr callback,
            JsArray::Ptr args) {
            return Ptr(new OnTimeout(callback, args));
        }

        Value operator()(JsArray::Ptr args) {
            (*callback_)(args_);
            return libj::Status::OK;
        }

     private:
        JsFunction::Ptr callback_;
        JsArray::Ptr args_;

        OnTimeout(
            JsFunction::Ptr callback,
            JsArray::Ptr args)
            : callback_(callback)
            , args_(args) {}
    };

}  // namespace

Value setTimeout(JsFunction::Ptr callback, Int delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay <= 0) delay = 1;
    uv::Timer* timer = new uv::Timer();
    timer->setOnTimeout(OnTimeout::create(callback, args));
    timer->start(delay, 0);
    return timer;
}

Value setInterval(JsFunction::Ptr callback, Int delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay <= 0) delay = 1;
    uv::Timer* timer = new uv::Timer();
    timer->setOnTimeout(OnTimeout::create(callback, args));
    timer->start(delay, delay);
    return timer;
}

void clearTimeout(Value timeoutId) {
    uv::Timer* timer = NULL;
    if (to<uv::Timer*>(timeoutId, &timer) && timer) {
        timer->setOnTimeout(JsFunction::null());
        timer->close();
    }
}

void clearInterval(Value intervalId) {
    uv::Timer* timer = NULL;
    if (to<uv::Timer*>(intervalId, &timer) && timer) {
        timer->setOnTimeout(JsFunction::null());
        timer->close();
    }
}

}  // namespace node
}  // namespace libj
