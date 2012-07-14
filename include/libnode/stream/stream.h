// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_STREAM_STREAM_H_
#define LIBNODE_STREAM_STREAM_H_

#include "libnode/events/event_emitter.h"

namespace libj {
namespace node {

class Stream : LIBNODE_EVENT_EMITTER(Stream)
 public:
    static const String::CPtr EVENT_ERROR;
    static const String::CPtr EVENT_CLOSE;

    virtual Boolean destroy() = 0;
};

#define LIBNODE_STREAM(T) public libj::node::Stream { \
    LIBJ_MUTABLE_DECLS(T, libj::node::Stream)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_STREAM_STREAM_H_
