// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_MESSAGE_QUEUE_H_
#define LIBNODE_MESSAGE_QUEUE_H_

#include <libnode/events/event_emitter.h>

namespace libj {
namespace node {

class MessageQueue : LIBNODE_EVENT_EMITTER(MessageQueue)
 public:
    static Symbol::CPtr EVENT_MESSAGE;

    static Ptr create();

    virtual Boolean start() = 0;

    virtual Boolean stop() = 0;

    virtual Boolean postMessage(const Value& msg) = 0;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_MESSAGE_QUEUE_H_
