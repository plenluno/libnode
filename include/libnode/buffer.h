// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BUFFER_H_
#define LIBNODE_BUFFER_H_

#include <libj/js_array_buffer.h>
#include <libj/js_typed_array.h>

namespace libj {
namespace node {

class Buffer : LIBJ_JS_ARRAY_BUFFER(Buffer)
 public:
    static Ptr create(Size length = 0);
    static Ptr create(const void* data, Size length);
    static Ptr create(JsTypedArray<UByte>::CPtr array);
    static Ptr create(
        String::CPtr str, String::Encoding enc = String::UTF8);

    static Boolean isBuffer(const Value& val);

    static Size byteLength(
        String::CPtr str, String::Encoding enc = String::UTF8);

    static Ptr concat(JsArray::CPtr list, Size total = NO_POS);

    virtual Int write(
        String::CPtr str,
        Size offset = 0,
        Size length = NO_POS,
        String::Encoding enc = String::UTF8) = 0;

    virtual Size copy(
        Ptr target,
        Size targetStart = 0,
        Size sourceStart = 0,
        Size sourceEnd = NO_POS) const = 0;

    virtual String::CPtr toString() const = 0;
    virtual String::CPtr toString(
        String::Encoding enc,
        Size start = 0,
        Size end = NO_POS) const = 0;

    virtual Int get(Size offset) {
        UByte b;
        if (readUInt8(offset, &b)) {
            return b;
        } else {
            return -1;
        }
    }

#ifdef LIBJ_USE_EXCEPTION
    UByte readUInt8(Size offset) const {
        return getUInt8(offset);
    }

    UShort readUInt16LE(Size offset) const {
        return getUInt16(offset, true);
    }

    UShort readUInt16BE(Size offset) const {
        return getUInt16(offset, false);
    }

    UInt readUInt32LE(Size offset) const {
        return getUInt32(offset, true);
    }

    UInt readUInt32BE(Size offset) const {
        return getUInt32(offset, false);
    }

    Byte readInt8(Size offset) const {
        return getInt8(offset);
    }

    Short readInt16LE(Size offset) const {
        return getInt16(offset, true);
    }

    Short readInt16BE(Size offset) const {
        return getInt16(offset, false);
    }

    Int readInt32LE(Size offset) const {
        return getInt32(offset, true);
    }

    Int readInt32BE(Size offset) const {
        return getInt32(offset, false);
    }

    Float readFloatLE(Size offset) const {
        return getFloat32(offset, true);
    }

    Float readFloatBE(Size offset) const {
        return getFloat32(offset, false);
    }

    Double readDoubleLE(Size offset) const {
        return getFloat64(offset, true);
    }

    Double readDoubleBE(Size offset) const {
        return getFloat64(offset, false);
    }
#endif  // LIBJ_USE_EXCEPTION

    Boolean readUInt8(Size offset, UByte* value) const {
        return getUInt8(offset, value);
    }

    Boolean readUInt16LE(Size offset, UShort* value) const {
        return getUInt16(offset, value, true);
    }

    Boolean readUInt16BE(Size offset, UShort* value) const {
        return getUInt16(offset, value, false);
    }

    Boolean readUInt32LE(Size offset, UInt* value) const {
        return getUInt32(offset, value, true);
    }

    Boolean readUInt32BE(Size offset, UInt* value) const {
        return getUInt32(offset, value, false);
    }

    Boolean readInt8(Size offset, Byte* value) const {
        return getInt8(offset, value);
    }

    Boolean readInt16LE(Size offset, Short* value) const {
        return getInt16(offset, value, true);
    }

    Boolean readInt16BE(Size offset, Short* value) const {
        return getInt16(offset, value, false);
    }

    Boolean readInt32LE(Size offset, Int* value) const {
        return getInt32(offset, value, true);
    }

    Boolean readInt32BE(Size offset, Int* value) const {
        return getInt32(offset, value, false);
    }

    Boolean readFloatLE(Size offset, Float* value) const {
        return getFloat32(offset, value, true);
    }

    Boolean readFloatBE(Size offset, Float* value) const {
        return getFloat32(offset, value, false);
    }

    Boolean readDoubleLE(Size offset, Double* value) const {
        return getFloat64(offset, value, true);
    }

    Boolean readDoubleBE(Size offset, Double* value) const {
        return getFloat64(offset, value, false);
    }

    Boolean writeUInt8(UByte value, Size offset) {
        return setUInt8(offset, value);
    }

    Boolean writeUInt16LE(UShort value, Size offset) {
        return setUInt16(offset, value, true);
    }

    Boolean writeUInt16BE(UShort value, Size offset) {
        return setUInt16(offset, value, false);
    }

    Boolean writeUInt32LE(UInt value, Size offset) {
        return setUInt32(offset, value, true);
    }

    Boolean writeUInt32BE(UInt value, Size offset) {
        return setUInt32(offset, value, false);
    }

    Boolean writeInt8(Byte value, Size offset) {
        return setInt8(offset, value);
    }

    Boolean writeInt16LE(Short value, Size offset) {
        return setInt16(offset, value, true);
    }

    Boolean writeInt16BE(Short value, Size offset) {
        return setInt16(offset, value, false);
    }

    Boolean writeInt32LE(Int value, Size offset) {
        return setInt32(offset, value, true);
    }

    Boolean writeInt32BE(Int value, Size offset) {
        return setInt32(offset, value, false);
    }

    Boolean writeFloatLE(Float value, Size offset) {
        return setFloat32(offset, value, true);
    }

    Boolean writeFloatBE(Float value, Size offset) {
        return setFloat32(offset, value, false);
    }

    Boolean writeDoubleLE(Double value, Size offset) {
        return setFloat64(offset, value, true);
    }

    Boolean writeDoubleBE(Double value, Size offset) {
        return setFloat64(offset, value, false);
    }
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BUFFER_H_
