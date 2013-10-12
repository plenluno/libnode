// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_DUPLEX_H_
#define LIBNODE_STREAM_DUPLEX_H_

#include <libnode/buffer.h>
#include <libnode/stream/stream.h>

namespace libj {
namespace node {
namespace stream {

class Duplex : LIBNODE_STREAM(Duplex)
 public:
    static Symbol::CPtr EVENT_DATA;
    static Symbol::CPtr EVENT_END;
    static Symbol::CPtr EVENT_DRAIN;
    static Symbol::CPtr EVENT_PIPE;

    // Readable
    virtual Boolean readable() const = 0;

    virtual Boolean pause() = 0;

    virtual Boolean resume() = 0;

    virtual Boolean setEncoding(Buffer::Encoding enc) = 0;

    // Writable
    virtual Boolean writable() const = 0;

    virtual Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean end(
        const Value& data = UNDEFINED,
        Buffer::Encoding enc = Buffer::NONE) = 0;

    virtual Boolean destroySoon() = 0;
};

}  // namespace stream
}  // namespace node
}  // namespace libj

#define LIBNODE_STREAM_DUPLEX(T) public libj::node::stream::Duplex { \
    LIBJ_MUTABLE_DEFS(T, libj::node::stream::Duplex)

#endif  // LIBNODE_STREAM_DUPLEX_H_
