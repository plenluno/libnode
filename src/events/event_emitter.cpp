// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/events/event_emitter.h>
#include <libnode/detail/events/event_emitter.h>

namespace libj {
namespace node {
namespace events {

LIBJ_SYMBOL_DEF(EventEmitter::EVENT_NEW_LISTENER, "newListener");

EventEmitter::Ptr EventEmitter::create() {
    return Ptr(new detail::events::EventEmitter<EventEmitter>());
}

}  // namespace events
}  // namespace node
}  // namespace libj
