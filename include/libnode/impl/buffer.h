// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_BUFFER_H_
#define LIBNODE_IMPL_BUFFER_H_

#include <libj/exception.h>
#include <libnode/util.h>

namespace libj {
namespace node {

inline Boolean Buffer::isBuffer(const Value& val) {
    return val.instanceof(Type<Buffer>::id());
}

inline Size Buffer::byteLength(String::CPtr str, Encoding enc) {
    if (str) {
        if (enc == NONE) enc = UTF8;
        return create(str, enc)->length();
    } else {
        return 0;
    }
}

inline Buffer::Ptr Buffer::concat(JsArray::CPtr list, Size total) {
    Size len = list->length();
    Size sum = 0;
    for (Size i = 0; i < len; i++) {
        CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) sum += b->length();
    }
    Size bufLen = sum < total ? sum : total;

    Ptr buf = create(bufLen);
    Size offset = 0;
    for (Size i = 0; i < len; i++) {
        CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) offset += b->copy(buf, offset);
    }
    return buf;
}

inline Buffer::Ptr Buffer::concat(Buffer::CPtr other) const {
    if (!other) return null();

    Size length = this->length();
    Ptr buf = create(length + other->length());
    this->copy(buf, 0);
    other->copy(buf, length);
    return buf;
}

inline Int Buffer::write(
    String::CPtr str,
    Size offset,
    Size length,
    Buffer::Encoding enc) {
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

inline Size Buffer::copy(
    Buffer::Ptr target,
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

inline UByte Buffer::at(Size offset) const {
    UByte b;
    if (!readUInt8(offset, &b)) {
        LIBJ_THROW(Error::INDEX_OUT_OF_BOUNDS);
    }
    return b;
}

inline Boolean Buffer::readUInt8(Size offset, UByte* value) const {
    return getUInt8(offset, value);
}

inline Boolean Buffer::readUInt16LE(Size offset, UShort* value) const {
    return getUInt16(offset, value, true);
}

inline Boolean Buffer::readUInt16BE(Size offset, UShort* value) const {
    return getUInt16(offset, value, false);
}

inline Boolean Buffer::readUInt32LE(Size offset, UInt* value) const {
    return getUInt32(offset, value, true);
}

inline Boolean Buffer::readUInt32BE(Size offset, UInt* value) const {
    return getUInt32(offset, value, false);
}

inline Boolean Buffer::readInt8(Size offset, Byte* value) const {
    return getInt8(offset, value);
}

inline Boolean Buffer::readInt16LE(Size offset, Short* value) const {
    return getInt16(offset, value, true);
}

inline Boolean Buffer::readInt16BE(Size offset, Short* value) const {
    return getInt16(offset, value, false);
}

inline Boolean Buffer::readInt32LE(Size offset, Int* value) const {
    return getInt32(offset, value, true);
}

inline Boolean Buffer::readInt32BE(Size offset, Int* value) const {
    return getInt32(offset, value, false);
}

inline Boolean Buffer::readFloatLE(Size offset, Float* value) const {
    return getFloat32(offset, value, true);
}

inline Boolean Buffer::readFloatBE(Size offset, Float* value) const {
    return getFloat32(offset, value, false);
}

inline Boolean Buffer::readDoubleLE(Size offset, Double* value) const {
    return getFloat64(offset, value, true);
}

inline Boolean Buffer::readDoubleBE(Size offset, Double* value) const {
    return getFloat64(offset, value, false);
}

inline Boolean Buffer::writeUInt8(UByte value, Size offset) {
    return setUInt8(offset, value);
}

inline Boolean Buffer::writeUInt16LE(UShort value, Size offset) {
    return setUInt16(offset, value, true);
}

inline Boolean Buffer::writeUInt16BE(UShort value, Size offset) {
    return setUInt16(offset, value, false);
}

inline Boolean Buffer::writeUInt32LE(UInt value, Size offset) {
    return setUInt32(offset, value, true);
}

inline Boolean Buffer::writeUInt32BE(UInt value, Size offset) {
    return setUInt32(offset, value, false);
}

inline Boolean Buffer::writeInt8(Byte value, Size offset) {
    return setInt8(offset, value);
}

inline Boolean Buffer::writeInt16LE(Short value, Size offset) {
    return setInt16(offset, value, true);
}

inline Boolean Buffer::writeInt16BE(Short value, Size offset) {
    return setInt16(offset, value, false);
}

inline Boolean Buffer::writeInt32LE(Int value, Size offset) {
    return setInt32(offset, value, true);
}

inline Boolean Buffer::writeInt32BE(Int value, Size offset) {
    return setInt32(offset, value, false);
}

inline Boolean Buffer::writeFloatLE(Float value, Size offset) {
    return setFloat32(offset, value, true);
}

inline Boolean Buffer::writeFloatBE(Float value, Size offset) {
    return setFloat32(offset, value, false);
}

inline Boolean Buffer::writeDoubleLE(Double value, Size offset) {
    return setFloat64(offset, value, true);
}

inline Boolean Buffer::writeDoubleBE(Double value, Size offset) {
    return setFloat64(offset, value, false);
}

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_IMPL_BUFFER_H_
