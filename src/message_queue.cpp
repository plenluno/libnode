// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/message_queue.h>
#include <libnode/detail/message_queue.h>

namespace libj {
namespace node {

LIBJ_SYMBOL_DEF(MessageQueue::EVENT_MESSAGE, "message");

MessageQueue::Ptr MessageQueue::create() {
    return Ptr(new detail::MessageQueue<MessageQueue>());
}

}  // namespace node
}  // namespace libj
