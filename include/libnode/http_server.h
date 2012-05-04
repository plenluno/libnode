// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_H_
#define LIBNODE_HTTP_SERVER_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {

class HttpServer : LIBNODE_EVENT_EMITTER(HttpServer)
public:
    static Type<String>::Cptr EVENT_REQUEST;
    static Type<String>::Cptr EVENT_CONNECTION;
    static Type<String>::Cptr EVENT_CLOSE;
    
    static Ptr create(Type<JsFunction>::Ptr requestListener);
    
    virtual bool listen(Int port) = 0;
    virtual void close() = 0;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_H_