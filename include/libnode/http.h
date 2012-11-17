// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_H_
#define LIBNODE_HTTP_H_

#include "libnode/http/agent.h"
#include "libnode/http/header.h"
#include "libnode/http/server.h"
#include "libnode/http/client_request.h"
#include "libnode/http/client_response.h"
#include "libnode/http/server_request.h"
#include "libnode/http/server_response.h"
#include "libnode/http/status.h"

namespace libj {
namespace node {
namespace http {

extern Agent::Ptr globalAgent;

Server::Ptr createServer(JsFunction::Ptr requestListener = JsFunction::null());

ClientRequest::Ptr request(
    JsObject::CPtr options = JsObject::null(),
    JsFunction::Ptr callback = JsFunction::null());

ClientRequest::Ptr get(
    JsObject::CPtr options = JsObject::null(),
    JsFunction::Ptr callback = JsFunction::null());

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_H_
