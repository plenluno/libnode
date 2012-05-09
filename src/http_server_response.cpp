// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_response_impl.h"

namespace libj {
namespace node {
namespace http {

String::CPtr ServerResponse::EVENT_CLOSE = String::create("close");

ServerResponse::Ptr ServerResponse::create() {
    return ServerResponseImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
