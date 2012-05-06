// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_response_impl.h"

namespace libj {
namespace node {
namespace http {

Type<String>::Cptr ServerResponseImpl::HEADERS = String::create("headers");
Type<String>::Cptr ServerResponseImpl::STATUS_CODE = String::create("statusCode");

}  // namespace http
}  // namespace node
}  // namespace libj
