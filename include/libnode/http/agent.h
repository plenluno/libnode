// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_AGENT_H_
#define LIBNODE_HTTP_AGENT_H_

#include <libnode/events/event_emitter.h>

namespace libj {
namespace node {
namespace http {

class Agent : LIBNODE_EVENT_EMITTER(Agent)
 public:
    static Ptr create(JsObject::CPtr options = JsObject::null());

    virtual Size maxSockets() const = 0;

    virtual void setMaxSockets(Size max) = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_AGENT_H_
