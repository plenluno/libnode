// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_EXAMPLE_HELLO_SERVER_H_
#define LIBNODE_EXAMPLE_HELLO_SERVER_H_

#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/trace.h>

namespace libj {
namespace node {
namespace example {

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
 public:
    virtual Value operator()(JsArray::Ptr args) {
        LIBJ_STATIC_CONST_STRING_DEF(TEXT_PLAIN,  "text/plain");
        LIBJ_STATIC_CONST_STRING_DEF(HELLO_WORLD, "Hello World\n");

        http::ServerResponse::Ptr res = args->getPtr<http::ServerResponse>(1);
        res->setHeader(http::HEADER_CONTENT_TYPE, TEXT_PLAIN);
        res->end(HELLO_WORLD);
        return UNDEFINED;
    }
};

inline void helloServer() {
    http::Server::Ptr srv =
        http::Server::create(JsFunction::Ptr(new OnRequest()));
    srv->listen(1337, String::create("127.0.0.1"));
    node::run();
}

}  // namespace exampel
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EXAMPLE_HELLO_SERVER_H_
