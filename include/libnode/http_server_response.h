// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace http {

class ServerResponse : LIBNODE_EVENT_EMITTER(ServerResponse)
public:
    static Type<String>::Cptr EVENT_CLOSE;
    
    virtual void writeHead(Int statusCode) = 0;
    virtual Int statusCode() const = 0;
    virtual void setHeader(Type<String>::Cptr name, Type<String>::Cptr value) = 0;
    virtual Type<String>::Cptr getHeader(Type<String>::Cptr name) const = 0;
    virtual void removeHeader(Type<String>::Cptr name) = 0;
    virtual void write(Type<Object>::Cptr chunk) = 0;
    virtual void end() = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_H_