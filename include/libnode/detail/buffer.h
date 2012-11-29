// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_BUFFER_H_
#define LIBNODE_DETAIL_BUFFER_H_

#include <libj/detail/js_array_buffer.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class Buffer : public libj::detail::JsArrayBuffer<I> {
 public:
    Buffer(Size length) : libj::detail::JsArrayBuffer<I>(length) {}
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_BUFFER_H_
