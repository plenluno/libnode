// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/buffer.h>
#include <libnode/detail/buffer.h>

#include <string>
#include <algorithm>

namespace libj {
namespace node {

Buffer::Ptr Buffer::create(Size length) {
    return Ptr(new detail::Buffer<Buffer>(length));
}

Buffer::Ptr Buffer::create(const void* data, Size length) {
    if (!data) return null();

    detail::Buffer<Buffer>* buf(new detail::Buffer<Buffer>(length));
    const UByte* d = static_cast<const UByte*>(data);
    for (Size i = 0; i < length; i++) {
        buf->setUInt8(i, d[i]);
    }
    return Ptr(buf);
}

Buffer::Ptr Buffer::create(TypedJsArray<UByte>::CPtr array) {
    if (!array) return null();

    Size length = array->length();
    detail::Buffer<Buffer>* buf(new detail::Buffer<Buffer>(length));
    for (Size i = 0; i < length; i++) {
        Value v = array->get(i);
        UByte b;
        to<UByte>(v, &b);
        buf->setUInt8(i, b);
    }
    return Ptr(buf);
}

Buffer::Ptr Buffer::create(String::CPtr str, Buffer::Encoding enc) {
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

String::CPtr Buffer::toString(
    Encoding enc,
    Size start,
    Size end) const {
    const Size size = length();
    if (end > size) end = size;
    if (start > end || start > size) return String::null();
    if (start == end) return String::create();

    Ptr buf;
    const void* dp;
    Size len;
    if (start == 0 && end == size) {
        dp = data();
        len = size;
    } else {
        buf = create(end - start);
        copy(buf, 0, start, end);
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

}  // namespace node
}  // namespace libj
