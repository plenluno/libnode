// Copyright (c) 2012-2014 Plenluno All rights reserved.

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

template<typename T>
static Buffer::Ptr createBuffer(T t, Buffer::Encoding enc) {
    if (!t) return Buffer::null();

    std::string s;
    switch (enc) {
    case Buffer::UTF8:
        s = t->toStdString();
        return Buffer::create(
            reinterpret_cast<const UByte*>(s.c_str()),
            s.length());
    case Buffer::UTF16BE:
        s = t->toStdString(String::UTF16BE);
        return Buffer::create(
            reinterpret_cast<const UByte*>(s.c_str()),
            s.length() - 2);  // delete the last null character
    case Buffer::UTF16LE:
        s = t->toStdString(String::UTF16LE);
        return Buffer::create(
            reinterpret_cast<const UByte*>(s.c_str()),
            s.length() - 2);  // delete the last null character
    case Buffer::UTF32BE:
        s = t->toStdString(String::UTF32BE);
        return Buffer::create(
            reinterpret_cast<const UByte*>(s.c_str()),
            s.length() - 4);  // delete the last null character
    case Buffer::UTF32LE:
        s = t->toStdString(String::UTF32LE);
        return Buffer::create(
            reinterpret_cast<const UByte*>(s.c_str()),
            s.length() - 4);  // delete the last null character
    case Buffer::BASE64:
        return util::base64Decode(t);
    case Buffer::HEX:
        return util::hexDecode(t);
    default:
        return Buffer::null();
    }
}

Buffer::Ptr Buffer::create(String::CPtr str, Buffer::Encoding enc) {
    return createBuffer<String::CPtr>(str, enc);
}

Buffer::Ptr Buffer::create(StringBuilder::CPtr sb, Buffer::Encoding enc) {
    return createBuffer<StringBuilder::CPtr>(sb, enc);
}

}  // namespace node
}  // namespace libj
