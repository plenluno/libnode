// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_REQ_H_
#define LIBNODE_DETAIL_UV_REQ_H_

#include <libj/js_function.h>

#include <uv.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

template<typename T>
class Req {
 public:
    Req() : onComplete(JsFunction::null()) {}

    Req(JsFunction::Ptr cb) : onComplete(cb) {}

    virtual ~Req() {}

    void dispatched() {
        req.data = this;
    }

    T req;
    JsFunction::Ptr onComplete;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_REQ_H_
