// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_BUFFER_H_
#define LIBNODE_DETAIL_BUFFER_H_

#include <libj/detail/js_array_buffer.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class Buffer : public I {
 public:
    typedef typename I::Ptr Ptr;
    typedef typename I::CPtr CPtr;

    Buffer(Size length)
        : buffer_(new libj::detail::JsArrayBuffer(length, false))
        , offset_(0)
        , length_(length) {}

    Buffer(
        libj::detail::JsArrayBuffer::Ptr buffer,
        Size offset,
        Size length)
        : buffer_(buffer)
        , offset_(offset)
        , length_(length) {}

    virtual Ptr concat(CPtr other) const {
        if (!other) return I::null();

        Ptr buf(new Buffer(length_ + other->length()));
        this->copy(buf, 0);
        other->copy(buf, length_);
        return buf;
    }

    virtual String::CPtr toString() const {
        return toString(I::UTF8);
    }

    virtual String::CPtr toString(
        typename I::Encoding enc,
        Size start = 0,
        Size end = NO_POS) const {
        if (end > length_) end = length_;
        if (start > end || start > length_) return String::null();
        if (start == end) return String::create();

        const Byte* dp = static_cast<const Byte*>(data()) + start;
        Size len = end - start;

        switch (enc) {
        case I::UTF8:
            return String::create(dp, String::UTF8,    len);
        case I::UTF16BE:
            return String::create(dp, String::UTF16BE, len >> 1);
        case I::UTF16LE:
            return String::create(dp, String::UTF16LE, len >> 1);
        case I::UTF32BE:
            return String::create(dp, String::UTF32BE, len >> 2);
        case I::UTF32LE:
            return String::create(dp, String::UTF32LE, len >> 2);
        case I::BASE64:
            return util::base64Encode(dp, len);
        case I::HEX:
            return util::hexEncode(dp, len);
        default:
            return String::null();
        }
    }

    virtual Ptr slice(Size start, Size end) const {
        if (end > length_) end = length_;
        if (start > end || start > length_) return I::null();

        return Ptr(new Buffer(buffer_, offset_ + start, end - start));
    }

    virtual Int write(
        String::CPtr str,
        Size offset,
        Size length,
        typename I::Encoding enc) {
        if (!str || offset > length_) {
            return -1;
        } else if (!length) {
            return 0;
        }

        std::string s;
        Size len;
        switch (enc) {
        case I::UTF8:
            s = str->toStdString();
            len = s.length();
            break;
        case I::UTF16BE:
            s = str->toStdString(String::UTF16BE);
            len = s.length() - 2;
            break;
        case I::UTF16LE:
            s = str->toStdString(String::UTF16LE);
            len = s.length() - 2;
            break;
        case I::UTF32BE:
            s = str->toStdString(String::UTF32BE);
            len = s.length() - 4;
            break;
        case I::UTF32LE:
            s = str->toStdString(String::UTF32LE);
            len = s.length() - 4;
            break;
        default:
            return -1;
        }

        Size remain = length_ - offset;
        len = len < length ? len : length;
        len = len < remain ? len : remain;
        const UByte* src = reinterpret_cast<const UByte*>(s.c_str());
        UByte* dst = static_cast<UByte*>(const_cast<void*>(buffer_->data()));
        dst += offset_ + offset;
        std::copy(src, src + len, dst);
        return len;
    }

    virtual Size copy(
        Ptr target,
        Size targetStart,
        Size sourceStart = 0,
        Size sourceEnd = NO_POS) const {
        if (!target) return 0;

        if (sourceEnd > length_) sourceEnd = length_;

        Size sourceLen = 0;
        if (sourceStart < sourceEnd) {
            sourceLen = sourceEnd - sourceStart;
        }

        Size copyLen = 0;
        if (sourceLen && targetStart < target->length()) {
            Size max = target->length() - targetStart;
            copyLen = sourceLen < max ? sourceLen : max;
        }

        const UByte* src = static_cast<const UByte*>(buffer_->data());
        UByte* dst = static_cast<UByte*>(const_cast<void*>(target->data()));
        src += sourceStart + offset_;
        dst += targetStart;
        std::copy(src, src + copyLen, dst);
        return copyLen;
    }

    virtual Size length() const {
        return length_;
    }

    virtual const void* data() const {
        return static_cast<const Byte*>(buffer_->data()) + offset_;
    }

    virtual Boolean readUInt8(Size offset, UByte* value) const {
        offset += offset_;
        return buffer_->getUint8(offset, value);
    }

    virtual Boolean readUInt16LE(Size offset, UShort* value) const {
        offset += offset_;
        return buffer_->getUint16(offset, value, true);
    }

    virtual Boolean readUInt16BE(Size offset, UShort* value) const {
        offset += offset_;
        return buffer_->getUint16(offset, value, false);
    }

    virtual Boolean readUInt32LE(Size offset, UInt* value) const {
        offset += offset_;
        return buffer_->getUint32(offset, value, true);
    }

    virtual Boolean readUInt32BE(Size offset, UInt* value) const {
        offset += offset_;
        return buffer_->getUint32(offset, value, false);
    }

    virtual Boolean readInt8(Size offset, Byte* value) const {
        offset += offset_;
        return buffer_->getInt8(offset, value);
    }

    virtual Boolean readInt16LE(Size offset, Short* value) const {
        offset += offset_;
        return buffer_->getInt16(offset, value, true);
    }

    virtual Boolean readInt16BE(Size offset, Short* value) const {
        offset += offset_;
        return buffer_->getInt16(offset, value, false);
    }

    virtual Boolean readInt32LE(Size offset, Int* value) const {
        offset += offset_;
        return buffer_->getInt32(offset, value, true);
    }

    virtual Boolean readInt32BE(Size offset, Int* value) const {
        offset += offset_;
        return buffer_->getInt32(offset, value, false);
    }

    virtual Boolean readFloatLE(Size offset, Float* value) const {
        offset += offset_;
        return buffer_->getFloat32(offset, value, true);
    }

    virtual Boolean readFloatBE(Size offset, Float* value) const {
        offset += offset_;
        return buffer_->getFloat32(offset, value, false);
    }

    virtual Boolean readDoubleLE(Size offset, Double* value) const {
        offset += offset_;
        return buffer_->getFloat64(offset, value, true);
    }

    virtual Boolean readDoubleBE(Size offset, Double* value) const {
        offset += offset_;
        return buffer_->getFloat64(offset, value, false);
    }

    virtual Boolean writeUInt8(UByte value, Size offset) {
        offset += offset_;
        return buffer_->setUint8(offset, value);
    }

    virtual Boolean writeUInt16LE(UShort value, Size offset) {
        offset += offset_;
        return buffer_->setUint16(offset, value, true);
    }

    virtual Boolean writeUInt16BE(UShort value, Size offset) {
        offset += offset_;
        return buffer_->setUint16(offset, value, false);
    }

    virtual Boolean writeUInt32LE(UInt value, Size offset) {
        offset += offset_;
        return buffer_->setUint32(offset, value, true);
    }

    virtual Boolean writeUInt32BE(UInt value, Size offset) {
        offset += offset_;
        return buffer_->setUint32(offset, value, false);
    }

    virtual Boolean writeInt8(Byte value, Size offset) {
        offset += offset_;
        return buffer_->setInt8(offset, value);
    }

    virtual Boolean writeInt16LE(Short value, Size offset) {
        offset += offset_;
        return buffer_->setInt16(offset, value, true);
    }

    virtual Boolean writeInt16BE(Short value, Size offset) {
        offset += offset_;
        return buffer_->setInt16(offset, value, false);
    }

    virtual Boolean writeInt32LE(Int value, Size offset) {
        offset += offset_;
        return buffer_->setInt32(offset, value, true);
    }

    virtual Boolean writeInt32BE(Int value, Size offset) {
        offset += offset_;
        return buffer_->setInt32(offset, value, false);
    }

    virtual Boolean writeFloatLE(Float value, Size offset) {
        offset += offset_;
        return buffer_->setFloat32(offset, value, true);
    }

    virtual Boolean writeFloatBE(Float value, Size offset) {
        offset += offset_;
        return buffer_->setFloat32(offset, value, false);
    }

    virtual Boolean writeDoubleLE(Double value, Size offset) {
        offset += offset_;
        return buffer_->setFloat64(offset, value, true);
    }

    virtual Boolean writeDoubleBE(Double value, Size offset) {
        offset += offset_;
        return buffer_->setFloat64(offset, value, false);
    }

 private:
    libj::detail::JsArrayBuffer::Ptr buffer_;
    Size offset_;
    Size length_;
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_BUFFER_H_
