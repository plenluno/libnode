// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_OPTION_H_
#define LIBNODE_HTTP_OPTION_H_

#include <libj/symbol.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_OPTION_MAP(GEN) \
    GEN(OPTION_HOST,          "host") \
    GEN(OPTION_HOSTNAME,      "hostname") \
    GEN(OPTION_PORT,          "port") \
    GEN(OPTION_LOCAL_ADDRESS, "localAddress") \
    GEN(OPTION_SOCKET_PATH,   "socketPath") \
    GEN(OPTION_METHOD,        "method") \
    GEN(OPTION_PATH,          "path") \
    GEN(OPTION_HEADERS,       "headers") \
    GEN(OPTION_AUTH,          "auth") \
    GEN(OPTION_AGENT,         "agent")

#define LIBNODE_HTTP_OPTION_DECL_GEN(NAME, VAL) \
    extern Symbol::CPtr NAME;

LIBNODE_HTTP_OPTION_MAP(LIBNODE_HTTP_OPTION_DECL_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_OPTION_H_
