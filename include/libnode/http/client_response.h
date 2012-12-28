// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_CLIENT_RESPONSE_H_
#define LIBNODE_HTTP_CLIENT_RESPONSE_H_

#include <libnode/stream/readable_stream.h>

namespace libj {
namespace node {
namespace http {

class ClientResponse : LIBNODE_READABLE_STREAM(ClientResponse)
 public:
    virtual Int statusCode() const = 0;

    virtual String::CPtr httpVersion() const = 0;

    virtual JsObject::CPtr headers() const = 0;

    virtual JsObject::CPtr trailers() const = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_CLIENT_RESPONSE_H_
