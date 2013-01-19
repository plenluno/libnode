// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_ASYNC_H_
#define LIBNODE_ASYNC_H_

#include <libj/executors.h>
#include <libj/js_object.h>
#include <libj/js_function.h>

namespace libj {
namespace node {

class Async : LIBJ_JS_OBJECT(Async)
 public:
    static Ptr create(
        Size numThreads,
        ThreadFactory::Ptr threadFactory = executors::defaultThreadFactory());

    virtual Boolean close() = 0;

    virtual Boolean exec(JsFunction::Ptr task, JsFunction::Ptr callback) = 0;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_ASYNC_H_
