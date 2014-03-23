// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_HTTP_AGENT_H_
#define LIBNODE_IMPL_HTTP_AGENT_H_

#define LIBNODE_HTTP_AGENT_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::http::Agent>::id() \
        || LIBNODE_EVENT_EMITTER_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_HTTP_AGENT_H_
