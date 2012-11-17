// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_WRITABLE_STREAM_H_
#define LIBNODE_STREAM_WRITABLE_STREAM_H_

#include "libnode/buffer.h"
#include "libnode/stream/stream.h"

namespace libj {
namespace node {

class WritableStream : LIBNODE_STREAM(WritableStream)
 public:
    static Symbol::CPtr EVENT_DRAIN;
    static Symbol::CPtr EVENT_PIPE;

    virtual Boolean writable() const = 0;

    virtual Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean end(
        const Value& data = UNDEFINED,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean destroySoon() = 0;
};

#define LIBNODE_WRITABLE_STREAM(T) public libj::node::WritableStream { \
    LIBJ_MUTABLE_DEFS(T, libj::node::WritableStream)

#define LIBNODE_WRITABLE_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
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

#endif  // LIBNODE_STREAM_WRITABLE_STREAM_H_
