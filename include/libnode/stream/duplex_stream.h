// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_DUPLEX_STREAM_H_
#define LIBNODE_STREAM_DUPLEX_STREAM_H_

#include "libnode/buffer.h"
#include "libnode/stream/stream.h"

namespace libj {
namespace node {

class DuplexStream : LIBNODE_STREAM(DuplexStream)
 public:
    static const String::CPtr EVENT_DATA;
    static const String::CPtr EVENT_END;
    static const String::CPtr EVENT_DRAIN;
    static const String::CPtr EVENT_PIPE;

    // ReadableStream
    virtual Boolean readable() const = 0;
    virtual Boolean setEncoding(Buffer::Encoding enc) = 0;

    // WritableStream
    virtual Boolean writable() const = 0;
    virtual Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) = 0;
    virtual Boolean end() = 0;
};

#define LIBNODE_DUPLEX_STREAM(T) public libj::node::DuplexStream { \
    LIBJ_MUTABLE_DEFS(T, libj::node::DuplexStream)

#define LIBNODE_DUPLEX_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean readable() const { \
        return S->readable(); \
    } \
    virtual Boolean setEncoding(Buffer::Encoding enc) { \
        return S->setEncoding(enc); \
    } \
    virtual Boolean writable() const { \
        return S->writable(); \
    } \
    virtual Boolean write( \
        const Value& data, \
        Buffer::Encoding enc = Buffer::UTF8) { \
        return S->write(data, enc); \
    } \
    virtual Boolean end() { \
        return S->end(); \
    }

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_DUPLEX_STREAM_H_
