// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_STREAM_ABSTRACT_DUPLEX_STREAM_H_
#define LIBNODE_BRIDGE_STREAM_ABSTRACT_DUPLEX_STREAM_H_

#include <libnode/stream/duplex_stream.h>
#include <libnode/bridge/stream/abstract_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace stream {

template<typename I>
class AbstractDuplexStream : public AbstractStream<I> {
 public:
    AbstractDuplexStream(DuplexStream::Ptr stream)
        : AbstractStream<I>(stream)
        , stream_(stream) {}

    // ReadableStream
    virtual Boolean readable() const {
        return stream_->readable();
    }

    virtual Boolean setEncoding(Buffer::Encoding enc) {
        return stream_->setEncoding(enc);
    }

    virtual Boolean pause() {
        return stream_->pause();
    }

    virtual Boolean resume() {
        return stream_->resume();
    }

    // WritableStream
    virtual Boolean writable() const {
        return stream_->writable();
    }

    virtual Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) {
        return stream_->write(data, enc);
    }

    virtual Boolean end(
        const Value& data = UNDEFINED,
        Buffer::Encoding enc = Buffer::NONE) {
        return stream_->end(data, enc);
    }

    virtual Boolean destroySoon() {
        return stream_->destroySoon();
    }

 private:
    DuplexStream::Ptr stream_;
};

}  // namespace stream
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_STREAM_ABSTRACT_DUPLEX_STREAM_H_
