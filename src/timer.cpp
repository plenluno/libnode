// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/timer.h"

#include "./uv/timer.h"

namespace libj {
namespace node {

namespace {

    const UInt TIMEOUT_MAX = 2147483647;  // 2^31-1

    class OnTimeout : LIBJ_JS_FUNCTION(OnTimeout)
     public:
        static Ptr create(
            JsFunction::Ptr callback,
            JsArray::Ptr args,
            uv::Timer* timer,
            Boolean repeat) {
            return Ptr(new OnTimeout(callback, args, timer, repeat));
        }

        Value operator()(JsArray::Ptr args) {
            (*callback_)(args_);
            if (!repeat_) {
                timer_->close();
            }
            return libj::Status::OK;
        }

     private:
        JsFunction::Ptr callback_;
        JsArray::Ptr args_;
        uv::Timer* timer_;
        Boolean repeat_;

        OnTimeout(
            JsFunction::Ptr callback,
            JsArray::Ptr args,
            uv::Timer* timer,
            Boolean repeat)
            : callback_(callback)
            , args_(args)
            , timer_(timer)
            , repeat_(repeat) {}
    };

}  // namespace

Value setTimeout(JsFunction::Ptr callback, UInt delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay == 0 || delay > TIMEOUT_MAX) delay = 1;
    uv::Timer* timer = new uv::Timer();
    timer->setOnTimeout(OnTimeout::create(callback, args, timer, false));
    timer->start(delay, 0);
    return timer;
}

Value setInterval(JsFunction::Ptr callback, UInt delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay == 0 || delay > TIMEOUT_MAX) delay = 1;
    uv::Timer* timer = new uv::Timer();
    timer->setOnTimeout(OnTimeout::create(callback, args, timer, true));
    timer->start(delay, delay);
    return timer;
}

Boolean clearTimeout(Value timeoutId) {
    uv::Timer* timer = NULL;
    if (to<uv::Timer*>(timeoutId, &timer) && timer) {
        timer->close();
        return true;
    } else {
        return false;
    }
}

Boolean clearInterval(Value intervalId) {
    uv::Timer* timer = NULL;
    if (to<uv::Timer*>(intervalId, &timer) && timer) {
        timer->close();
        return true;
    } else {
        return false;
    }
}

}  // namespace node
}  // namespace libj
