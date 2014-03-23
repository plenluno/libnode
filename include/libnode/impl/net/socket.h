// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_NET_SOCKET_H_
#define LIBNODE_IMPL_NET_SOCKET_H_

#define LIBNODE_NET_SOCKET_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::net::Socket>::id() \
        || LIBNODE_STREAM_DUPLEX_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_NET_SOCKET_H_
