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
        return Ptr(new BufferImpl(length));
    }

    static Ptr create(const void* data, Size length) {
        if (!data) return null();

        BufferImpl* buf(new BufferImpl(length));
        const UByte* d = static_cast<const UByte*>(data);
        for (Size i = 0; i < length; i++)
            buf->setUInt8(i, d[i]);
        return Ptr(buf);
    }

    static Ptr create(JsTypedArray<UByte>::CPtr array) {
        if (!array) return null();

        Size length = array->length();
        BufferImpl* buf(new BufferImpl(length));
        for (Size i = 0; i < length; i++) {
            Value v = array->get(i);
            UByte b;
            to<UByte>(v, &b);
            buf->setUInt8(i, b);
        }
        return Ptr(buf);
    }

    static Ptr create(String::CPtr str, Encoding enc) {
        if (!str) return null();

        std::string s;
        switch (enc) {
        case UTF8:
            s = str->toStdString();
            return create(
                reinterpret_cast<const UByte*>(s.c_str()),
                s.length());
        case UTF16BE:
            s = str->toStdString(String::UTF16BE);
            return create(
                reinterpret_cast<const UByte*>(s.c_str()),
                s.length() - 2);  // delete the last null character
        case UTF16LE:
            s = str->toStdString(String::UTF16LE);
            return create(
                reinterpret_cast<const UByte*>(s.c_str()),
                s.length() - 2);  // delete the last null character
        case UTF32BE:
            s = str->toStdString(String::UTF32BE);
            return create(
                reinterpret_cast<const UByte*>(s.c_str()),
                s.length() - 4);  // delete the last null character
        case UTF32LE:
            s = str->toStdString(String::UTF32LE);
            return create(
                reinterpret_cast<const UByte*>(s.c_str()),
                s.length() - 4);  // delete the last null character
        case BASE64:
            return util::base64Decode(str);
        case HEX:
            return util::hexDecode(str);
        default:
            return null();
        }
    }

    Ptr concat(CPtr other) const {
        if (!other) return null();

        Size length = this->length();
        Buffer::Ptr buf = Buffer::create(length + other->length());
        this->copy(buf, 0);
        other->copy(buf, length);
        return buf;
    }

    Int write(
        String::CPtr str, Size offset, Size length, Encoding enc) {
        if (!str || offset > this->length()) {
            return -1;
        } else if (!length) {
            return 0;
        }

        std::string s;
        Size len;
        switch (enc) {
        case UTF8:
            s = str->toStdString();
            len = s.length();
            break;
        case UTF16BE:
            s = str->toStdString(String::UTF16BE);
            len = s.length() - 2;
            break;
        case UTF16LE:
            s = str->toStdString(String::UTF16LE);
            len = s.length() - 2;
            break;
        case UTF32BE:
            s = str->toStdString(String::UTF32BE);
            len = s.length() - 4;
            break;
        case UTF32LE:
            s = str->toStdString(String::UTF32LE);
            len = s.length() - 4;
            break;
        default:
            return -1;
        }

        Size remain = this->length() - offset;
        len = len < length ? len : length;
        len = len < remain ? len : remain;
        const UByte* data = reinterpret_cast<const UByte*>(s.c_str());
        for (Size i = 0; i < len; i++) {
            setUInt8(i + offset, data[i]);
        }
        return len;
    }

    virtual Value slice(Size begin, Size end) const {
        Value val = buffer_->slice(begin, end);
        JsArrayBuffer::Ptr buf = toPtr<JsArrayBuffer>(val);
        return Ptr(new BufferImpl(buf));
    }

    virtual Size copy(
        Ptr target,
        Size targetStart = 0,
        Size sourceStart = 0,
        Size sourceEnd = NO_POS) const {
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

        Buffer::Ptr buf;
        const void* dp;
        Size len;
        if (start == 0 && end == size) {
            dp = data();
            len = size;
        } else {
            buf = Buffer::create(end - start);
            this->copy(buf, 0, start, end);
            dp = buf->data();
            len = buf->length();
        }

        switch (enc) {
        case UTF8:
            return String::create(dp, String::UTF8);
        case UTF16BE:
            return String::create(dp, String::UTF16BE);
        case UTF16LE:
            return String::create(dp, String::UTF16LE);
        case UTF32BE:
            return String::create(dp, String::UTF32BE);
        case UTF32LE:
            return String::create(dp, String::UTF32LE);
        case BASE64:
            return util::base64Encode(dp, len);
        case HEX:
            return util::hexEncode(dp, len);
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
        if (enc == NONE) enc = UTF8;
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
