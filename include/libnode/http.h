// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_H_
#define LIBNODE_HTTP_H_

#include <libnode/http/agent.h>
#include <libnode/http/header.h>
#include <libnode/http/server.h>
#include <libnode/http/client_request.h>
#include <libnode/http/client_response.h>
#include <libnode/http/server_request.h>
#include <libnode/http/server_response.h>
#include <libnode/http/status.h>

namespace libj {
namespace node {
namespace http {

Agent::Ptr globalAgent();

Server::Ptr createServer(JsFunction::Ptr requestListener = JsFunction::null());

ClientRequest::Ptr request(const Value& options, JsFunction::Ptr callback);

ClientRequest::Ptr get(const Value& options, JsFunction::Ptr callback);

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_H_
