// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_DUPLEX_STREAM_H_
#define LIBNODE_STREAM_DUPLEX_STREAM_H_

#include "libnode/buffer.h"
#include "libnode/stream/stream.h"

namespace libj {
namespace node {

class DuplexStream : LIBNODE_STREAM(DuplexStream)
 public:
    static Symbol::CPtr EVENT_DATA;
    static Symbol::CPtr EVENT_END;
    static Symbol::CPtr EVENT_DRAIN;
    static Symbol::CPtr EVENT_PIPE;

    // ReadableStream
    virtual Boolean readable() const = 0;

    virtual Boolean pause() = 0;

    virtual Boolean resume() = 0;

    virtual Boolean setEncoding(Buffer::Encoding enc) = 0;

    // WritableStream
    virtual Boolean writable() const = 0;

    virtual Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean end(
        const Value& data = UNDEFINED,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean destroySoon() = 0;
};

#define LIBNODE_DUPLEX_STREAM(T) public libj::node::DuplexStream { \
    LIBJ_MUTABLE_DEFS(T, libj::node::DuplexStream)

#define LIBNODE_DUPLEX_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean readable() const { \
        return S->readable(); \
    } \
    virtual Boolean pause() { \
        return S->pause(); \
    } \
    virtual Boolean resume() { \
        return S->resume(); \
    } \
    virtual Boolean setEncoding(Buffer::Encoding enc) { \
        return S->setEncoding(enc); \
    } \
    virtual Boolean writable() const { \
        return S->writable(); \
    } \
    virtual Boolean write( \
        const Value& data, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->write(data, enc); \
    } \
    virtual Boolean end( \
        const Value& data = UNDEFINED, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->end(data, enc); \
    } \
    virtual Boolean destroySoon() { \
        return S->destroySoon(); \
    }

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_DUPLEX_STREAM_H_
