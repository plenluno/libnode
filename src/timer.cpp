// Copyright (c) 2012 Plenluno All rights reserved.

#include <uv.h>
#include <map>

#include "libnode/timer.h"

namespace libj {
namespace node {

namespace {
    Int nextTimeoutId = 1;
    Int nextIntervalId = -1;

    struct TimerContext {
        Int id;
        uv_timer_t timer;
        int64_t timeout;
        JsFunction::Ptr callback;
        JsArray::Ptr args;
        bool isCleared;
    };

    std::map<Int, TimerContext*> timers;

    void clearTimer(Value timerId) {
        Int id;
        if (!to<Int>(timerId, &id))
            return;
        std::map<Int, TimerContext*>::const_iterator itr = timers.find(id);
        if (itr != timers.end()) {
            TimerContext* context = itr->second;
            uv_timer_stop(&context->timer);
            timers.erase(id);
            delete context;
            uv_unref(uv_default_loop());
        }
    }

    void onTimeout(uv_timer_t* handle, int status) {
        TimerContext* context = static_cast<TimerContext*>(handle->data);
        if (!context->isCleared)
            (*context->callback)(context->args);
        clearTimer(context->id);
    }

    void onInterval(uv_timer_t* handle, int status) {
        TimerContext* context = static_cast<TimerContext*>(handle->data);
        if (context->isCleared) {
            clearTimer(context->id);
        } else {
            (*context->callback)(context->args);
            uv_timer_stop(&context->timer);
            uv_timer_start(&context->timer, onInterval, context->timeout, 1);
        }
    }

    Int setTimer(
        Int id,
        Int delay,
        JsFunction::Ptr callback,
        JsArray::Ptr args,
        uv_timer_cb cb) {
        TimerContext* context = new TimerContext;
        context->id = id;
        context->timeout = delay < 0 ? 0 : delay;
        context->callback = callback;
        context->args = args;
        context->isCleared = false;
        timers[context->id] = context;
        uv_timer_init(uv_default_loop(), &context->timer);
        context->timer.data = context;
        uv_timer_start(&context->timer, cb, context->timeout, 1);
        return context->id;
    }
}

Value setTimeout(JsFunction::Ptr callback, Int delay, JsArray::Ptr args) {
    return setTimer(nextTimeoutId++, delay, callback, args, onTimeout);
}

Value setInterval(JsFunction::Ptr callback, Int delay, JsArray::Ptr args) {
    return setTimer(nextIntervalId--, delay, callback, args, onInterval);
}

void clearTimeout(Value timeoutId) {
    Int id;
    if (!to<Int>(timeoutId, &id))
        return;
    std::map<Int, TimerContext*>::const_iterator itr = timers.find(id);
    if (itr != timers.end()) {
        TimerContext* context = itr->second;
        context->isCleared = true;
    }
}

void clearInterval(Value intervalId) {
    clearTimeout(intervalId);
}

}  // namespace node
}  // namespace libj
