// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/string_decoder.h>

#include <libj/js_object.h>
#include <libj/bridge/abstract_js_object.h>

namespace libj {
namespace node {

typedef bridge::AbstractJsObject<StringDecoder> StringDecoderBase;

class StringDecoderImpl : public StringDecoderBase {
 public:
    static Ptr create(Buffer::Encoding enc) {
        return Ptr(new StringDecoderImpl(enc));
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
    Buffer::Encoding enc_;

    StringDecoderImpl(Buffer::Encoding enc)
        : StringDecoderBase(JsObject::create())
        , enc_(enc) {}
};

StringDecoder::Ptr StringDecoder::create(Buffer::Encoding enc) {
    return StringDecoderImpl::create(enc);
}

}  // namespace node
}  // namespace libj
