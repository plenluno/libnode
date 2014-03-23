// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_

#define LIBNODE_EVENT_EMITTER_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::events::EventEmitter>::id() \
        || LIBJ_JS_OBJECT_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_
