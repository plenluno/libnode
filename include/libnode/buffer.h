// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_BUFFER_H_
#define LIBNODE_BUFFER_H_

#include <libj/typed_js_array.h>

namespace libj {
namespace node {

class Buffer : LIBJ_MUTABLE(Buffer)
 public:
    enum Encoding {
        UTF8,
        UTF16BE,
        UTF16LE,
        UTF32BE,
        UTF32LE,
        BASE64,
        HEX,
        NONE,
    };

    static Ptr create(Size length = 0);

    static Ptr create(const void* data, Size length);

    static Ptr create(TypedJsArray<UByte>::CPtr array);

    static Ptr create(String::CPtr str, Encoding enc = UTF8);

    static Boolean isBuffer(const Value& val);

    static Size byteLength(String::CPtr str, Encoding enc = UTF8);

    static Ptr concat(JsArray::CPtr list, Size total = NO_SIZE);

    virtual Ptr concat(CPtr other) const = 0;

    virtual String::CPtr toString() const = 0;

    virtual String::CPtr toString(
        Encoding enc, Size start = 0, Size end = NO_POS) const = 0;

    virtual Ptr slice(Size start = 0, Size end = NO_POS) const = 0;

    virtual Int write(
        String::CPtr str,
        Size offset = 0,
        Size length = NO_SIZE,
        Encoding enc = UTF8) = 0;

    virtual Size copy(
        Ptr target,
        Size targetStart = 0,
        Size sourceStart = 0,
        Size sourceEnd = NO_POS) const = 0;

    virtual Size length() const = 0;

    virtual const void* data() const = 0;

    virtual UByte at(Size offset) const = 0;

    virtual Boolean readUInt8(Size offset, UByte* value) const = 0;

    virtual Boolean readUInt16LE(Size offset, UShort* value) const = 0;

    virtual Boolean readUInt16BE(Size offset, UShort* value) const = 0;

    virtual Boolean readUInt32LE(Size offset, UInt* value) const = 0;

    virtual Boolean readUInt32BE(Size offset, UInt* value) const = 0;

    virtual Boolean readInt8(Size offset, Byte* value) const = 0;

    virtual Boolean readInt16LE(Size offset, Short* value) const = 0;

    virtual Boolean readInt16BE(Size offset, Short* value) const = 0;

    virtual Boolean readInt32LE(Size offset, Int* value) const = 0;

    virtual Boolean readInt32BE(Size offset, Int* value) const = 0;

    virtual Boolean readFloatLE(Size offset, Float* value) const = 0;

    virtual Boolean readFloatBE(Size offset, Float* value) const = 0;

    virtual Boolean readDoubleLE(Size offset, Double* value) const = 0;

    virtual Boolean readDoubleBE(Size offset, Double* value) const = 0;

    virtual Boolean writeUInt8(UByte value, Size offset) = 0;

    virtual Boolean writeUInt16LE(UShort value, Size offset) = 0;

    virtual Boolean writeUInt16BE(UShort value, Size offset) = 0;

    virtual Boolean writeUInt32LE(UInt value, Size offset) = 0;

    virtual Boolean writeUInt32BE(UInt value, Size offset) = 0;

    virtual Boolean writeInt8(Byte value, Size offset) = 0;

    virtual Boolean writeInt16LE(Short value, Size offset) = 0;

    virtual Boolean writeInt16BE(Short value, Size offset) = 0;

    virtual Boolean writeInt32LE(Int value, Size offset) = 0;

    virtual Boolean writeInt32BE(Int value, Size offset) = 0;

    virtual Boolean writeFloatLE(Float value, Size offset) = 0;

    virtual Boolean writeFloatBE(Float value, Size offset) = 0;

    virtual Boolean writeDoubleLE(Double value, Size offset) = 0;

    virtual Boolean writeDoubleBE(Double value, Size offset) = 0;
};

}  // namespace node
}  // namespace libj

#include <libnode/impl/buffer.h>

#endif  // LIBNODE_BUFFER_H_
