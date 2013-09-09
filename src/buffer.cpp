// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/buffer.h>
#include <libnode/detail/buffer.h>

namespace libj {
namespace node {

Buffer::Ptr Buffer::create(Size length) {
    return Ptr(new detail::Buffer<Buffer>(length));
}

Buffer::Ptr Buffer::create(const void* data, Size length) {
    if (!data) return null();

    detail::Buffer<Buffer>* buf(new detail::Buffer<Buffer>(length));
    const UByte* src = static_cast<const UByte*>(data);
    UByte* dst = static_cast<UByte*>(const_cast<void*>(buf->data()));
    std::copy(src, src + length, dst);
    return Ptr(buf);
}

Buffer::Ptr Buffer::create(TypedJsArray<UByte>::CPtr array) {
    if (!array) return null();

    Size length = array->length();
    detail::Buffer<Buffer>* buf(new detail::Buffer<Buffer>(length));
    for (Size i = 0; i < length; i++) {
        buf->writeUInt8(array->getTyped(i), i);
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

}  // namespace node
}  // namespace libj
