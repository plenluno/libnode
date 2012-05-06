// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

Type<String>::Cptr ServerRequestImpl::METHOD = String::create("method");
Type<String>::Cptr ServerRequestImpl::URL = String::create("url");
Type<String>::Cptr ServerRequestImpl::HEADERS = String::create("headers");
Type<String>::Cptr ServerRequestImpl::HTTP_VERSION = String::create("httpVerion");

}  // namespace http
}  // namespace node
}  // namespace libj
