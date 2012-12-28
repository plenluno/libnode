// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_READABLE_STREAM_H_
#define LIBNODE_STREAM_READABLE_STREAM_H_

#include <libnode/buffer.h>
#include <libnode/stream/stream.h>

namespace libj {
namespace node {

class ReadableStream : LIBNODE_STREAM(ReadableStream)
 public:
    static Symbol::CPtr EVENT_DATA;
    static Symbol::CPtr EVENT_END;

    virtual Boolean readable() const = 0;

    virtual Boolean pause() = 0;

    virtual Boolean resume() = 0;

    virtual Boolean setEncoding(Buffer::Encoding enc) = 0;
};

}  // namespace node
}  // namespace libj

#define LIBNODE_READABLE_STREAM(T) public libj::node::ReadableStream { \
    LIBJ_MUTABLE_DEFS(T, libj::node::ReadableStream)

#endif  // LIBNODE_STREAM_READABLE_STREAM_H_
