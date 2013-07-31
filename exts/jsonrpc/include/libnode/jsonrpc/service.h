// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_SERVICE_H_
#define LIBNODE_JSONRPC_SERVICE_H_

#include <libnode/jsonrpc/method.h>
#include <libnode/events/event_emitter.h>

namespace libj {
namespace node {
namespace jsonrpc {

class Service : LIBNODE_EVENT_EMITTER(Service)
 public:
    static Symbol::CPtr EVENT_RESPONSE;

    static Ptr create();

    virtual void close() = 0;

    virtual Boolean addMethod(Method::CPtr method) = 0;

    virtual Boolean postRequest(String::CPtr req) = 0;

    virtual Boolean postRequest(JsObject::CPtr req) = 0;
};

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_SERVICE_H_
