// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_REQUEST_H_
#define LIBNODE_HTTP_SERVER_REQUEST_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace http {

class ServerRequest : LIBNODE_EVENT_EMITTER(ServerRequest)
public:
    static Type<String>::Cptr EVENT_DATA;
    static Type<String>::Cptr EVENT_END;
    static Type<String>::Cptr EVENT_CLOSE;
    
    virtual Type<String>::Cptr method() const = 0;
    virtual Type<String>::Cptr url() const = 0;
    virtual Type<JsObject>::Cptr headers() const = 0;
    virtual Type<String>::Cptr httpVersion() const = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_REQUEST_H_