// Copyright (c) 2012 Plenluno All rights reserved.

#include "libj/js_object.h"
#include "libnode/string_decoder.h"

namespace libj {
namespace node {

class StringDecoderImpl : public StringDecoder {
 public:
    static Ptr create(String::Encoding enc) {
        Ptr p(new StringDecoderImpl(enc));
        return p;
    }

    String::CPtr write(Buffer::CPtr buf) const {
        if (buf)
            return buf->toString(enc_);
        else
            return String::create();
    }

 private:
    JsObject::Ptr obj_;
    String::Encoding enc_;

    StringDecoderImpl(String::Encoding enc)
        : obj_(JsObject::create())
        , enc_(enc) {}

    LIBJ_JS_OBJECT_IMPL(obj_);
};

StringDecoder::Ptr StringDecoder::create(String::Encoding enc) {
    return StringDecoderImpl::create(enc);
}

}  // namespace node
}  // namespace libj
