// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_STREAM_ABSTRACT_STREAM_H_
#define LIBNODE_BRIDGE_STREAM_ABSTRACT_STREAM_H_

#include <libnode/stream/stream.h>
#include <libnode/bridge/events/abstract_event_emitter.h>

namespace libj {
namespace node {
namespace bridge {
namespace stream {

template<typename I>
class AbstractStream : public events::AbstractEventEmitter<I> {
 public:
    AbstractStream(Stream::Ptr stream)
        : events::AbstractEventEmitter<I>(stream)
        , stream_(stream) {}

    virtual Boolean destroy() {
        return stream_->destroy();
    }

 private:
    Stream::Ptr stream_;
};

}  // namespace stream
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_STREAM_ABSTRACT_STREAM_H_
