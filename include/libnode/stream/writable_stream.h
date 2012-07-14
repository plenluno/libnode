// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_WRITABLE_STREAM_H_
#define LIBNODE_STREAM_WRITABLE_STREAM_H_

#include "libnode/buffer.h"
#include "libnode/stream/stream.h"

namespace libj {
namespace node {

class WritableStream : LIBNODE_STREAM(WritableStream)
 public:
    static const String::CPtr EVENT_DRAIN;
    static const String::CPtr EVENT_PIPE;

    virtual Boolean writable() const = 0;

    virtual void write(Buffer::CPtr buf) = 0;
    virtual void write(String::CPtr str, String::Encoding enc = String::UTF8) = 0;
    virtual Boolean end() = 0;

    virtual Boolean destroySoon() = 0;

    Boolean end(String::CPtr str) {
        write(str);
        return end();
    }

    Boolean end(Buffer::CPtr buf) {
        write(buf);
        return end();
    }
};

#define LIBNODE_WRITABLE_STREAM(T) public libj::node::WritableStream { \
    LIBJ_MUTABLE_DECLS(T, libj::node::WritableStream)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_WRITABLE_STREAM_H_
