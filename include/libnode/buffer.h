// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BUFFER_H_
#define LIBNODE_BUFFER_H_

#include <libj/js_array_buffer.h>
#include <libj/typed_js_array.h>

namespace libj {
namespace node {

class Buffer : LIBJ_JS_ARRAY_BUFFER(Buffer)
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

    virtual String::CPtr toString() const = 0;

    Ptr concat(CPtr other) const;

    String::CPtr toString(
        Encoding enc, Size start = 0, Size end = NO_POS) const;

    Int write(
        String::CPtr str,
        Size offset = 0,
        Size length = NO_SIZE,
        Encoding enc = UTF8);

    Size copy(
        Ptr target,
        Size targetStart = 0,
        Size sourceStart = 0,
        Size sourceEnd = NO_POS) const;

    UByte at(Size offset) const;

    Boolean readUInt8(Size offset, UByte* value) const;

    Boolean readUInt16LE(Size offset, UShort* value) const;

    Boolean readUInt16BE(Size offset, UShort* value) const;

    Boolean readUInt32LE(Size offset, UInt* value) const;

    Boolean readUInt32BE(Size offset, UInt* value) const;

    Boolean readInt8(Size offset, Byte* value) const;

    Boolean readInt16LE(Size offset, Short* value) const;

    Boolean readInt16BE(Size offset, Short* value) const;

    Boolean readInt32LE(Size offset, Int* value) const;

    Boolean readInt32BE(Size offset, Int* value) const;

    Boolean readFloatLE(Size offset, Float* value) const;

    Boolean readFloatBE(Size offset, Float* value) const;

    Boolean readDoubleLE(Size offset, Double* value) const;

    Boolean readDoubleBE(Size offset, Double* value) const;

    Boolean writeUInt8(UByte value, Size offset);

    Boolean writeUInt16LE(UShort value, Size offset);

    Boolean writeUInt16BE(UShort value, Size offset);

    Boolean writeUInt32LE(UInt value, Size offset);

    Boolean writeUInt32BE(UInt value, Size offset);

    Boolean writeInt8(Byte value, Size offset);

    Boolean writeInt16LE(Short value, Size offset);

    Boolean writeInt16BE(Short value, Size offset);

    Boolean writeInt32LE(Int value, Size offset);

    Boolean writeInt32BE(Int value, Size offset);

    Boolean writeFloatLE(Float value, Size offset);

    Boolean writeFloatBE(Float value, Size offset);

    Boolean writeDoubleLE(Double value, Size offset);

    Boolean writeDoubleBE(Double value, Size offset);
};

}  // namespace node
}  // namespace libj

#include <libnode/impl/buffer.h>

#endif  // LIBNODE_BUFFER_H_
