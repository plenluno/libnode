// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_HTTP_SERVER_H_
#define LIBNODE_IMPL_HTTP_SERVER_H_

#define LIBNODE_HTTP_SERVER_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::http::Server>::id() \
        || LIBNODE_NET_SERVER_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_HTTP_SERVER_H_
