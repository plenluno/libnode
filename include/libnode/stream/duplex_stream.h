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

    virtual Boolean readable() const = 0;
    virtual Boolean writable() const = 0;

    // stream.setEncoding([encoding])
    // stream.pause()
    // stream.resume()
    // stream.pipe(destination, [options])

    virtual void write(Buffer::CPtr buf) = 0;
    virtual void write(
        String::CPtr str, String::Encoding enc = String::UTF8) = 0;
    virtual Boolean end() = 0;

    Boolean end(String::CPtr str) {
        write(str);
        return end();
    }

    Boolean end(Buffer::CPtr buf) {
        write(buf);
        return end();
    }
};

#define LIBNODE_DUPLEX_STREAM(T) public libj::node::DuplexStream { \
    LIBJ_MUTABLE_DECLS(T, libj::node::DuplexStream)

#define LIBNODE_DUPLEX_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean readable() const { \
        return S->readable(); \
    } \
    virtual Boolean writable() const { \
        return S->writable(); \
    } \
    virtual void write(Buffer::CPtr buf) { \
        return S->write(buf); \
    } \
    virtual void write( \
        String::CPtr str, String::Encoding enc = String::UTF8) { \
        S->write(str, enc); \
    } \
    virtual Boolean end() { \
        return S->end(); \
    }

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_DUPLEX_STREAM_H_
