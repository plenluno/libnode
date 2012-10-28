// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_H_
#define LIBNODE_HTTP_SERVER_H_

#include "libnode/net/server.h"

namespace libj {
namespace node {
namespace http {

class Server : LIBNODE_NET_SERVER(Server)
 public:
    static const String::CPtr EVENT_REQUEST;
    static const String::CPtr EVENT_CHECK_CONTINUE;
    static const String::CPtr EVENT_CONNECT;
    static const String::CPtr EVENT_UPGRADE;
    static const String::CPtr EVENT_CLIENT_ERROR;

    static Ptr create();
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_H_
