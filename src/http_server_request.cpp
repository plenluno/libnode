// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

Type<String>::Cptr ServerRequest::EVENT_DATA = String::create("data");
Type<String>::Cptr ServerRequest::EVENT_END = String::create("end");
Type<String>::Cptr ServerRequest::EVENT_CLOSE = String::create("close");

Type<ServerRequest>::Ptr ServerRequest::create() {
    return ServerRequestImpl::create();
}


}  // namespace http
}  // namespace node
}  // namespace libj
