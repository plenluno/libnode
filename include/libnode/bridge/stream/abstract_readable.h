// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_H_
#define LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_H_

#include <libnode/stream/readable.h>
#include <libnode/bridge/stream/abstract_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace stream {

template<typename I>
class AbstractReadable : public AbstractStream<I> {
 public:
    AbstractReadable(node::stream::Readable::Ptr stream)
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
    node::stream::Readable::Ptr stream_;
};

}  // namespace stream
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_STREAM_ABSTRACT_READABLE_H_
