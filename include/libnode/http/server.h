// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_H_
#define LIBNODE_HTTP_SERVER_H_

#include "libnode/net/server.h"

namespace libj {
namespace node {
namespace http {

class Server : LIBNODE_NET_SERVER(Server)
 public:
    static Symbol::CPtr EVENT_REQUEST;
    static Symbol::CPtr EVENT_CHECK_CONTINUE;
    static Symbol::CPtr EVENT_CONNECT;
    static Symbol::CPtr EVENT_UPGRADE;
    static Symbol::CPtr EVENT_CLIENT_ERROR;

    static Ptr create(JsFunction::Ptr requestListener = JsFunction::null());

    virtual Size maxHeadersCount() const = 0;

    virtual void setMaxHeadersCount(Size max) = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_H_
