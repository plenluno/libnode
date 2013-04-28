// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_METHOD_H_
#define LIBNODE_HTTP_METHOD_H_

#include <libj/symbol.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_METHOD_MAP(GEN) \
    GEN(METHOD_DELETE,  "DELETE") \
    GEN(METHOD_GET,     "GET") \
    GEN(METHOD_HEAD,    "HEAD") \
    GEN(METHOD_POST,    "POST") \
    GEN(METHOD_PUT,     "PUT") \
    GEN(METHOD_CONNECT, "CONNECT") \
    GEN(METHOD_OPTIONS, "OPTIONS") \
    GEN(METHOD_TRACE,   "TRACE")

#define LIBNODE_HTTP_METHOD_DECL_GEN(NAME, VAL) \
    extern Symbol::CPtr NAME;

LIBNODE_HTTP_METHOD_MAP(LIBNODE_HTTP_METHOD_DECL_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_METHOD_H_
