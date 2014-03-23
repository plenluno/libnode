// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_NET_SERVER_H_
#define LIBNODE_IMPL_NET_SERVER_H_

#define LIBNODE_NET_SERVER_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::net::Server>::id() \
        || LIBNODE_EVENT_EMITTER_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_NET_SERVER_H_
