// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace http {

class ServerResponse : LIBNODE_EVENT_EMITTER(ServerResponse)
public:
    static String::CPtr EVENT_CLOSE;
    
    virtual void writeHead(Int statusCode) = 0;
    virtual Int statusCode() const = 0;
    virtual void setHeader(String::CPtr name, String::CPtr value) = 0;
    virtual String::CPtr getHeader(String::CPtr name) const = 0;
    virtual void removeHeader(String::CPtr name) = 0;
    virtual void write(Object::CPtr chunk) = 0;
    virtual void end() = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_H_