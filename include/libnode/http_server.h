// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_H_
#define LIBNODE_HTTP_SERVER_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace http {

class Server : LIBNODE_EVENT_EMITTER(Server)
 public:
    static String::CPtr IN_ADDR_ANY;
    static String::CPtr EVENT_REQUEST;
    static String::CPtr EVENT_CONNECTION;
    static String::CPtr EVENT_CLOSE;

    static Ptr create(JsFunction::Ptr requestListener);

    virtual bool listen(Int port, String::CPtr hostName = IN_ADDR_ANY) = 0;
    virtual void close() = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_H_
