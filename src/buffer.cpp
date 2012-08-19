// Copyright (c) 2012 Plenluno All rights reserved.

#include <string>

#include "libnode/buffer.h"

namespace libj {
namespace node {

class BufferImpl : public Buffer {
 public:
    static Ptr create(Size length) {
        Ptr p(new BufferImpl(length));
        return p;
    }

    static Ptr create(const void* data, Size length) {
        Ptr p(new BufferImpl(length));
        const UByte* d = static_cast<const UByte*>(data);
        for (Size i = 0; i < length; i++)
            p->setUInt8(i, d[i]);
        return p;
    }

    static Ptr create(JsTypedArray<UByte>::CPtr array) {
        Size length = array->length();
        Ptr p(new BufferImpl(length));
        for (Size i = 0; i < length; i++) {
            Value v = array->get(i);
            UByte b;
            to<UByte>(v, &b);
            p->setUInt8(i, b);
        }
        return p;
    }

    static Ptr create(String::CPtr str, String::Encoding enc) {
        if (enc == String::ASCII || enc == String::UTF8) {
            std::string str8 = str->toStdString();
            Size length = str8.length();
            return create(
                reinterpret_cast<const UByte*>(str8.c_str()), length);
        } else {
            return null();
        }
    }

    Int write(
        String::CPtr str, Size offset, Size length, String::Encoding enc) {
        if (offset > this->length()) {
            return -1;
        } else if (enc == String::ASCII || enc == String::UTF8) {
            std::string str8 = str->toStdString();
            const UByte* data = reinterpret_cast<const UByte*>(str8.c_str());
            Size len = this->length() - offset;
            len = len < length ? len : length;
            len = len < str->length() ? len : str->length();
            for (Size i = 0; i < len; i++) {
                setUInt8(i + offset, data[i]);
            }
            return len;
        } else {
            return -1;
        }
    }

    virtual Value slice(Size begin, Size end) const {
        Value val = buffer_->slice(begin, end);
        JsArrayBuffer::Ptr buf = toPtr<JsArrayBuffer>(val);
        Ptr p(new BufferImpl(buf));
        return p;
    }

    virtual Size copy(
        Ptr target,
        Size targetStart,
        Size sourceStart,
        Size sourceEnd) const {
        if (sourceEnd > length())
            sourceEnd = length();
        Size sourceLen = 0;
        if (sourceStart < sourceEnd) {
            sourceLen = sourceEnd - sourceStart;
        }
        Size copyLen = 0;
        if (sourceLen && targetStart < target->length()) {
            Size max = target->length() - targetStart;
            copyLen = sourceLen < max ? sourceLen : max;
        }
        for (Size i = 0; i < copyLen; i++) {
            UByte value;
            getUInt8(sourceStart + i, &value);
            target->setUInt8(targetStart + i, value);
        }
        return copyLen;
    }

    virtual String::CPtr toString(
        String::Encoding enc,
        Size start,
        Size end) const {
        if (start == 0 && end >= length()) {
            return String::create(data(), enc);
        } else {
            Size len = end - start;
            Buffer::Ptr buf = Buffer::create(len);
            this->copy(buf, 0, start, end);
            return String::create(buf->data(), enc);
        }
    }

 private:
    JsArrayBuffer::Ptr buffer_;

    BufferImpl(Size size) : buffer_(JsArrayBuffer::create(size)) {}

    BufferImpl(JsArrayBuffer::Ptr buf) : buffer_(buf) {}

    LIBJ_JS_ARRAY_BUFFER_IMPL(buffer_);
};

Buffer::Ptr Buffer::create(Size length) {
    return BufferImpl::create(length);
}

Buffer::Ptr Buffer::create(const void* data, Size length) {
    return BufferImpl::create(data, length);
}

Buffer::Ptr Buffer::create(JsTypedArray<UByte>::CPtr array) {
    return BufferImpl::create(array);
}

Buffer::Ptr Buffer::create(String::CPtr str, String::Encoding enc) {
    return BufferImpl::create(str, enc);
}

Boolean Buffer::isBuffer(const Value& val) {
    return val.instanceof(Type<Buffer>::id());
}

Size Buffer::byteLength(String::CPtr str, String::Encoding enc) {
    if (str) {
        return create(str, enc)->length();
    } else {
        return 0;
    }
}

Buffer::Ptr Buffer::concat(JsArray::CPtr list, Size total) {
    Size len = list->length();
    Size sum = 0;
    for (Size i = 0; i < len; i++) {
        Buffer::CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) sum += b->length();
    }
    Size bufLen = sum < total ? sum : total;

    Buffer::Ptr buf = Buffer::create(bufLen);
    Size offset = 0;
    for (Size i = 0; i < len; i++) {
        Buffer::CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) offset += b->copy(buf, offset);
    }
    return buf;
}

}  // namespace node
}  // namespace libj
