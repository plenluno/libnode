// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/buffer.h"

namespace libj {
namespace node {

class BufferImpl : public Buffer {
 public:
    static Ptr create(Size size) {
        Ptr p(new BufferImpl(size));
        return p;
    }

    void write(
        String::CPtr str,
        Size offset = 0,
        Size length = NO_POS,
        String::Encoding enc = String::UTF8) {
        // TODO(KotaHanda): implement
    }

    virtual JsArrayBuffer::Ptr slice(Size begin) const {
        Ptr p(new BufferImpl(buffer_->slice(begin)));
        return p;
    }

    virtual JsArrayBuffer::Ptr slice(Size begin, Size end) const {
        Ptr p(new BufferImpl(buffer_->slice(begin, end)));
        return p;
    }

 private:
    JsArrayBuffer::Ptr buffer_;

    BufferImpl(Size size) : buffer_(JsArrayBuffer::create(size)) {}

    BufferImpl(JsArrayBuffer::Ptr buf) : buffer_(buf) {}

    LIBJ_JS_ARRAY_BUFFER_IMPL(buffer_);
};

Buffer::Ptr Buffer::create(Size size) {
    return BufferImpl::create(size);
}

}  // namespace node
}  // namespace libj
