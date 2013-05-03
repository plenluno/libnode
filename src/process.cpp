// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/process.h>
#include <libnode/timer.h>
#include <libnode/debug_print.h>

#include <libj/status.h>
#include <libj/typed_linked_list.h>

namespace libj {
namespace node {
namespace process {

class NextTick : LIBJ_JS_FUNCTION(NextTick)
 public:
    typedef TypedLinkedList<JsFunction::Ptr> CallbackQueue;

    NextTick() : queue_(CallbackQueue::create()) {
        LIBJ_DEBUG_PRINT(
            "static: NextTick::queue_ %p",
            LIBJ_DEBUG_OBJECT_PTR(queue_));
    }

    Value operator()(JsArray::Ptr args) {
        Size len = queue_->length();
        for (Size i = 0; i < len; i++) {
            JsFunction::Ptr cb = queue_->shiftTyped();
            (*cb)();
        }
        LIBNODE_DEBUG_PRINT("nextTick: %d", queue_->length());
        return Status::OK;
    }

    void push(JsFunction::Ptr cb) {
        queue_->pushTyped(cb);
        LIBNODE_DEBUG_PRINT("nextTick: %d", queue_->length());
    }

 private:
    CallbackQueue::Ptr queue_;
};

void nextTick(JsFunction::Ptr callback) {
    // TODO(plenluno): implement without setTimeout
    static NextTick::Ptr nt = NextTick::null();
    if (!nt) {
        nt = NextTick::Ptr(new NextTick());
        LIBJ_DEBUG_PRINT(
            "static: NextTick %p",
            LIBJ_DEBUG_OBJECT_PTR(nt));
    }

    if (callback) nt->push(callback);
    setTimeout(nt, 0);
}

}  // namespace process
}  // namespace node
}  // namespace libj
