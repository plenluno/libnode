// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <string>

#include "libnode/buffer.h"
#include "libnode/util.h"

namespace libj {
namespace node {

class BufferImpl : public Buffer {
 public:
    static Ptr create(Size length) {
        Ptr p(new BufferImpl(length));
        return p;
    }

    static Ptr create(const void* data, Size length) {
        if (!data) return null();

        Ptr p(new BufferImpl(length));
        const UByte* d = static_cast<const UByte*>(data);
        for (Size i = 0; i < length; i++)
            p->setUInt8(i, d[i]);
        return p;
    }

    static Ptr create(JsTypedArray<UByte>::CPtr array) {
        if (!array) return null();

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

    static Ptr create(String::CPtr str, Encoding enc) {
        if (!str) return null();

        std::string str8;
        switch (enc) {
        case BASE64:
            return util::base64Decode(str);
        case HEX:
            return util::hexDecode(str);
        case UTF8:
            str8 = str->toStdString();
            return create(
                reinterpret_cast<const UByte*>(str8.c_str()),
                str8.length());
        default:
            return null();
        }
    }

    Int write(
        String::CPtr str, Size offset, Size length, Encoding enc) {
        if (!str || offset > this->length()) return -1;

        if (enc == UTF8) {
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
        if (!target) return 0;

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
        Encoding enc,
        Size start,
        Size end) const {
        const Size size = length();
        if (end > size) end = size;
        if (start > end || start > size) return String::null();
        if (start == end) return String::create();
        
        if (start == 0 && end == size) {
            switch (enc) {
            case ASCII:
                return String::create(data(), String::ASCII);
            case UTF8:
                return String::create(data(), String::UTF8);
            default:
                break;
            }
        }

        Buffer::Ptr buf = Buffer::create(end - start);
        this->copy(buf, 0, start, end);
        switch (enc) {
        case ASCII:
            return String::create(buf->data(), String::ASCII);
        case UTF8:
            return String::create(buf->data(), String::UTF8);
        case BASE64:
            return util::base64Encode(buf);
        case HEX:
            return util::hexEncode(buf);
        default:
        return String::null();
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

Buffer::Ptr Buffer::create(String::CPtr str, Encoding enc) {
    return BufferImpl::create(str, enc);
}

Boolean Buffer::isBuffer(const Value& val) {
    return val.instanceof(Type<Buffer>::id());
}

Size Buffer::byteLength(String::CPtr str, Encoding enc) {
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
