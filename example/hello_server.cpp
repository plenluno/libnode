// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/config.h>

#ifdef LIBNODE_USE_CXX11
# include "./hello_server_cxx11.h"
#else
# include "./hello_server.h"
#endif

int main() {
    LIBNODE_DEBUG_TRACE_ON;
    namespace node = libj::node;
    node::example::helloServer();
    LIBNODE_DEBUG_TRACE_OFF;
    return 0;
}
