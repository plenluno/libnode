// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/http/client_request.h>

namespace libj {
namespace node {
namespace http {

LIBJ_SYMBOL_DEF(ClientRequest::EVENT_RESPONSE, "response");
LIBJ_SYMBOL_DEF(ClientRequest::EVENT_SOCKET,   "socket");
LIBJ_SYMBOL_DEF(ClientRequest::EVENT_CONNECT,  "connect");
LIBJ_SYMBOL_DEF(ClientRequest::EVENT_UPGRADE,  "upgrade");
LIBJ_SYMBOL_DEF(ClientRequest::EVENT_CONTINUE, "continue");

}  // namespace http
}  // namespace node
}  // namespace libj
