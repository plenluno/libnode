// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/detail/http/server.h>

namespace libj {
namespace node {
namespace http {

LIBJ_SYMBOL_DEF(Server::EVENT_REQUEST,        "request");
LIBJ_SYMBOL_DEF(Server::EVENT_CHECK_CONTINUE, "checkContinue");
LIBJ_SYMBOL_DEF(Server::EVENT_CONNECT,        "connect");
LIBJ_SYMBOL_DEF(Server::EVENT_UPGRADE,        "upgrade");
LIBJ_SYMBOL_DEF(Server::EVENT_CLIENT_ERROR,   "clientError");

Server::Ptr Server::create(JsFunction::Ptr requestListener) {
    return detail::http::Server::create(requestListener);
}

}  // namespace http
}  // namespace node
}  // namespace libj
