// Copyright (c) 2012 Plenluno All rights reserved.

#include "libj/js_object.h"
#include "libnode/string_decoder.h"

namespace libj {
namespace node {

class StringDecoderImpl : public StringDecoder {
 public:
    static Ptr create(Buffer::Encoding enc) {
        Ptr p(new StringDecoderImpl(enc));
        return p;
    }

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
    JsObject::Ptr obj_;
    Buffer::Encoding enc_;

    StringDecoderImpl(Buffer::Encoding enc)
        : obj_(JsObject::create())
        , enc_(enc) {}

    LIBJ_JS_OBJECT_IMPL(obj_);
};

StringDecoder::Ptr StringDecoder::create(Buffer::Encoding enc) {
    return StringDecoderImpl::create(enc);
}

}  // namespace node
}  // namespace libj
