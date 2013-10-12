// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_CLIENT_REQUEST_H_
#define LIBNODE_HTTP_CLIENT_REQUEST_H_

#include <libnode/stream/writable.h>

namespace libj {
namespace node {
namespace http {

class ClientRequest : LIBNODE_STREAM_WRITABLE(ClientRequest)
 public:
    static Symbol::CPtr EVENT_RESPONSE;
    static Symbol::CPtr EVENT_SOCKET;
    static Symbol::CPtr EVENT_CONNECT;
    static Symbol::CPtr EVENT_UPGRADE;
    static Symbol::CPtr EVENT_CONTINUE;

    virtual void abort() = 0;

    virtual void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual void setNoDelay(Boolean noDelay = true) = 0;

    virtual void setSocketKeepAlive(
        Boolean enable = false, UInt initialDelay = 0) = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_CLIENT_REQUEST_H_
