// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SERVER_H_
#define LIBNODE_NET_SERVER_H_

#include <libnode/net/socket.h>

namespace libj {
namespace node {
namespace net {

class Server : LIBNODE_EVENT_EMITTER(Server)
 public:
    static Symbol::CPtr IN_ADDR_ANY;
    static Symbol::CPtr EVENT_CLOSE;
    static Symbol::CPtr EVENT_ERROR;
    static Symbol::CPtr EVENT_LISTENING;
    static Symbol::CPtr EVENT_CONNECTION;

    static Ptr create();

    virtual Value address() = 0;

    virtual Size connections() const = 0;

    virtual Size maxConnections() const = 0;

    virtual void setMaxConnections(Size max) = 0;

    virtual Boolean listen(
        Int port,
        String::CPtr host = IN_ADDR_ANY,
        Int backlog = 511,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean listen(
        String::CPtr path,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean close(
        JsFunction::Ptr callback = JsFunction::null()) = 0;
};

}  // namespace net
}  // namespace node
}  // namespace libj

#define LIBNODE_NET_SERVER(T) public libj::node::net::Server { \
    LIBJ_MUTABLE_DEFS(T, libj::node::net::Server)

#endif  // LIBNODE_NET_SERVER_H_
