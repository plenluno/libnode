// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_response_impl.h"

namespace libj {
namespace node {
namespace http {

uv_buf_t* ServerResponseImpl::responseBuf = 0;

Type<String>::Cptr ServerResponseImpl::STATUS_CODE = String::create("statusCode");

}  // namespace http
}  // namespace node
}  // namespace libj
