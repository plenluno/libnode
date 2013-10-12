// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_WRITABLE_H_
#define LIBNODE_STREAM_WRITABLE_H_

#include <libnode/buffer.h>
#include <libnode/stream/stream.h>

namespace libj {
namespace node {
namespace stream {

class Writable : LIBNODE_STREAM(Writable)
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

}  // namespace stream
}  // namespace node
}  // namespace libj

#define LIBNODE_STREAM_WRITABLE(T) public libj::node::stream::Writable { \
    LIBJ_MUTABLE_DEFS(T, libj::node::stream::Writable)

#endif  // LIBNODE_STREAM_WRITABLE_H_
