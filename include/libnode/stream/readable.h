// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_READABLE_H_
#define LIBNODE_STREAM_READABLE_H_

#include <libnode/buffer.h>
#include <libnode/stream/stream.h>

namespace libj {
namespace node {
namespace stream {

class Readable : LIBNODE_STREAM(Readable)
 public:
    static Symbol::CPtr EVENT_DATA;
    static Symbol::CPtr EVENT_END;

    virtual Boolean readable() const = 0;

    virtual Boolean pause() = 0;

    virtual Boolean resume() = 0;

    virtual Boolean setEncoding(Buffer::Encoding enc) = 0;
};

}  // namespace stream
}  // namespace node
}  // namespace libj

#define LIBNODE_STREAM_READABLE(T) public libj::node::stream::Readable { \
    LIBJ_MUTABLE_DEFS(T, libj::node::stream::Readable)

#endif  // LIBNODE_STREAM_READABLE_H_
