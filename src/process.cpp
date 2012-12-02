// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/process.h>
#include <libnode/timer.h>

#include <libj/status.h>

namespace libj {
namespace node {
namespace process {

class NextTick : LIBJ_JS_FUNCTION(NextTick)
 public:
    NextTick() : queue_(JsArray::create()) {}

    Value operator()(JsArray::Ptr args) {
        Size len = queue_->length();
        for (Size i = 0; i < len; i++) {
            JsFunction::Ptr cb = queue_->getPtr<JsFunction>(i);
            (*cb)();
        }
        queue_->clear();
        return Status::OK;
    }

    void push(JsFunction::Ptr cb) {
        queue_->push(cb);
    }

 private:
    JsArray::Ptr queue_;
};

void nextTick(JsFunction::Ptr callback) {
    // TODO(plenluno): implement without setTimeout
    static NextTick::Ptr nt(new NextTick());
    if (callback) nt->push(callback);
    setTimeout(nt, 0);
}

}  // namespace process
}  // namespace node
}  // namespace libj
