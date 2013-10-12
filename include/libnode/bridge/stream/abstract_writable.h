// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_STREAM_ABSTRACT_WRITABLE_H_
#define LIBNODE_BRIDGE_STREAM_ABSTRACT_WRITABLE_H_

#include <libnode/stream/writable.h>
#include <libnode/bridge/stream/abstract_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace stream {

template<typename I>
class AbstractWritable : public AbstractStream<I> {
 public:
    AbstractWritable(node::stream::Writable::Ptr stream)
        : AbstractStream<I>(stream)
        , stream_(stream) {}

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
    node::stream::Writable::Ptr stream_;
};

}  // namespace stream
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_STREAM_ABSTRACT_WRITABLE_H_
