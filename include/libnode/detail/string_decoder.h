// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_STRING_DECODER_H_
#define LIBNODE_DETAIL_STRING_DECODER_H_

#include <libnode/buffer.h>
#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class StringDecoder : public libj::detail::JsObject<I> {
 public:
 	StringDecoder(Buffer::Encoding enc) : enc_(enc) {}

 	Buffer::Encoding encoding() const {
        return enc_;
    }

    String::CPtr write(Buffer::CPtr buf) const {
        if (buf)
            return buf->toString(enc_);
        else
            return String::create();
    }

 private:
    Buffer::Encoding enc_;
 };

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_STRING_DECODER_H_
