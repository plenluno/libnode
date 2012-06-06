// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

const String::CPtr ServerRequest::EVENT_DATA = String::create("data");
const String::CPtr ServerRequest::EVENT_END = String::create("end");
const String::CPtr ServerRequest::EVENT_CLOSE = String::create("close");

}  // namespace http
}  // namespace node
}  // namespace libj
