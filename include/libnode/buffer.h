// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BUFFER_H_
#define LIBNODE_BUFFER_H_

#include <libj/js_array_buffer.h>

namespace libj {
namespace node {

class Buffer : LIBJ_JS_ARRAY_BUFFER(Buffer)
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

    void writeUInt8(UByte value, Size offset) {
        setUInt8(offset, value);
    }

    void writeUInt16LE(UShort value, Size offset) {
        setUInt16(offset, value, true);
    }

    void writeUInt16BE(UShort value, Size offset) {
        setUInt16(offset, value, false);
    }

    void writeUInt32LE(UInt value, Size offset) {
        setUInt32(offset, value, true);
    }

    void writeUInt32BE(UInt value, Size offset) {
        setUInt32(offset, value, false);
    }
    
    void writeInt8(Byte value, Size offset) {
        setInt8(offset, value);
    }

    void writeInt16LE(Short value, Size offset) {
        setInt16(offset, value, true);
    }

    void writeInt16BE(Short value, Size offset) {
        setInt16(offset, value, false);
    }

    void writeInt32LE(Int value, Size offset) {
        setInt32(offset, value, true);
    }

    void writeInt32BE(Int value, Size offset) {
        setInt32(offset, value, false);
    }

    void writeFloatLE(Float value, Size offset) {
        setFloat32(offset, value, true);
    }

    void writeFloatBE(Float value, Size offset) {
        setFloat32(offset, value, false);
    }

    void writeDoubleLE(Double value, Size offset) {
        setFloat64(offset, value, true);
    }

    void writeDoubleBE(Double value, Size offset) {
        setFloat64(offset, value, false);
    }
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BUFFER_H_
