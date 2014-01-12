// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_TIMER_H_
#define LIBNODE_DETAIL_UV_TIMER_H_

#include <libnode/invoke.h>
#include <libnode/detail/uv/handle.h>
#include <libnode/detail/uv/write.h>

#include <libj/detail/gc_delete.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Timer : public Handle {
 public:
    Timer()
        : Handle(reinterpret_cast<uv_handle_t*>(&timer_))
        , onTimeout_(JsFunction::null()) {
        Int r = uv_timer_init(uv_default_loop(), &timer_);
        assert(r == 0);
        timer_.data = this;
    }

    virtual ~Timer() {
        LIBJ_GC_DELETE(onTimeout_);
    }

    Int start(Long timeout, Long repeat) {
        Int r = uv_timer_start(&timer_, onTimeout, timeout, repeat);
        if (r) setLastError();
        return r;
    }

    Int stop() {
        Int r = uv_timer_stop(&timer_);
        if (r) setLastError();
        return r;
    }

    Int again() {
        Int r = uv_timer_again(&timer_);
        if (r) setLastError();
        return r;
    }

    Int setRepeat(Long repeat) {
        uv_timer_set_repeat(&timer_, repeat);
        return 0;
    }

    Long getRepeat() {
        Long repeat = uv_timer_get_repeat(&timer_);
        if (repeat < 0) setLastError();
        return repeat;
    }

    void setOnTimeout(JsFunction::Ptr callback) {
        onTimeout_ = callback;
    }

 private:
    static void onTimeout(uv_timer_t* handle, int status) {
        Timer* self = static_cast<Timer*>(handle->data);
        if (self->onTimeout_) invoke(self->onTimeout_, status);
    }

 private:
    uv_timer_t timer_;
    JsFunction::Ptr onTimeout_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_TIMER_H_
