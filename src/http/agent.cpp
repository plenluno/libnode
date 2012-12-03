// Copyright (c) 2012 Plenluno All rights reserved.

#include "./agent_impl.h"

namespace libj {
namespace node {
namespace http {

LIBJ_SYMBOL_DEF(AgentImpl::EVENT_CLOSE, "close");
LIBJ_SYMBOL_DEF(AgentImpl::EVENT_FREE, "free");
LIBJ_SYMBOL_DEF(AgentImpl::EVENT_AGENT_REMOVE, "agentRemove");

const UInt AgentImpl::defaultPort_ = 80;
const Size AgentImpl::defaultMaxSockets_ = 5;

Agent::Ptr Agent::create(JsObject::CPtr options) {
    return AgentImpl::create(options);
}

}  // namespace http
}  // namespace node
}  // namespace libj
