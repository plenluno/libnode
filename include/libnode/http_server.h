// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_H_
#define LIBNODE_HTTP_SERVER_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {

class HttpServer : LIBNODE_EVENT_EMITTER(HttpServer)
public:
    static Ptr create(Type<JsFunction>::Ptr requestListener);
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_H_