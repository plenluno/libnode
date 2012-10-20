// Copyright (c) 2012 Plenluno All rights reserved.

#include "./server_response_impl.h"

namespace libj {
namespace node {
namespace http {

const String::CPtr ServerResponseImpl::HEADERS =
    String::intern("headers");
const String::CPtr ServerResponseImpl::STATUS_CODE =
    String::intern("statusCode");

}  // namespace http
}  // namespace node
}  // namespace libj
