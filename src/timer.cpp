// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/timer.h>

#include "./uv/timer.h"

namespace libj {
namespace node {

namespace {

    const UInt TIMEOUT_MAX = 2147483647;  // 2^31-1

    class OnTimeout : LIBJ_JS_FUNCTION(OnTimeout)
     public:
        OnTimeout(
            JsFunction::Ptr callback,
            JsArray::Ptr args,
            uv::Timer* timer,
            Boolean repeat)
            : callback_(callback)
            , args_(args)
            , timer_(timer)
            , repeat_(repeat) {}

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
    };

}  // namespace

Value setTimeout(JsFunction::Ptr callback, UInt delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay == 0 || delay > TIMEOUT_MAX) delay = 1;
    uv::Timer* timer = new uv::Timer();
    OnTimeout::Ptr onTimeout(new OnTimeout(callback, args, timer, false));
    timer->setOnTimeout(onTimeout);
    timer->start(delay, 0);
    return timer;
}

Value setInterval(JsFunction::Ptr callback, UInt delay, JsArray::Ptr args) {
    if (!callback) return UNDEFINED;

    if (delay == 0 || delay > TIMEOUT_MAX) delay = 1;
    uv::Timer* timer = new uv::Timer();
    OnTimeout::Ptr onTimeout(new OnTimeout(callback, args, timer, true));
    timer->setOnTimeout(onTimeout);
    timer->start(delay, delay);
    return timer;
}

Boolean clearTimeout(const Value& timeoutId) {
    uv::Timer* timer = NULL;
    if (to<uv::Timer*>(timeoutId, &timer) && timer) {
        timer->close();
        return true;
    } else {
        return false;
    }
}

Boolean clearInterval(const Value& intervalId) {
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
