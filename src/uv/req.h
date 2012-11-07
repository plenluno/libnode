// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_UV_REQ_H_
#define LIBNODE_SRC_UV_REQ_H_

#include <assert.h>
#include <uv.h>
#include <libj/js_function.h>

namespace libj {
namespace node {
namespace uv {

template<typename T>
class Req {
 public:
    Req() : onComplete(JsFunction::null()) {}

    virtual ~Req() {}

    void dispatched() {
        req.data = this;
    }

    T req;
    JsFunction::Ptr onComplete;
};

}  // namespace uv
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_UV_REQ_H_
