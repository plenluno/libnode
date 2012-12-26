// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_STREAM_H_
#define LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_STREAM_H_

#include <libnode/stream/readable_stream.h>
#include <libnode/bridge/stream/abstract_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace stream {

template<typename I>
class AbstractReadableStream : public AbstractStream<I> {
 public:
    AbstractReadableStream(ReadableStream::Ptr stream)
        : AbstractStream<I>(stream)
        , stream_(stream) {}

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

 private:
    ReadableStream::Ptr stream_;
};

}  // namespace stream
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_STREAM_H_
