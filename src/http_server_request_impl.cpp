// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

String::CPtr ServerRequestImpl::METHOD = String::create("method");
String::CPtr ServerRequestImpl::URL = String::create("url");
String::CPtr ServerRequestImpl::HEADERS = String::create("headers");
String::CPtr ServerRequestImpl::HTTP_VERSION = String::create("httpVerion");

}  // namespace http
}  // namespace node
}  // namespace libj
