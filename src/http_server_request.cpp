// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

String::CPtr ServerRequest::EVENT_DATA = String::create("data");
String::CPtr ServerRequest::EVENT_END = String::create("end");
String::CPtr ServerRequest::EVENT_CLOSE = String::create("close");

ServerRequest::Ptr ServerRequest::create() {
    return ServerRequestImpl::create();
}


}  // namespace http
}  // namespace node
}  // namespace libj
