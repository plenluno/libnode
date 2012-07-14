// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_READABLE_STREAM_H_
#define LIBNODE_STREAM_READABLE_STREAM_H_

#include "libnode/stream/stream.h"

namespace libj {
namespace node {

class ReadableStream : LIBNODE_STREAM(ReadableStream)
 public:
    static const String::CPtr EVENT_DATA;
    static const String::CPtr EVENT_END;

    virtual Boolean readable() const = 0;

    // stream.setEncoding([encoding])
    // stream.pause()
    // stream.resume()
    // stream.pipe(destination, [options])
};

#define LIBNODE_READABLE_STREAM(T) public libj::node::ReadableStream { \
    LIBJ_MUTABLE_DECLS(T, libj::node::ReadableStream)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_READABLE_STREAM_H_
