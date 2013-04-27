// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_NET_OPTION_H_
#define LIBNODE_NET_OPTION_H_

#include <libj/symbol.h>

namespace libj {
namespace node {
namespace net {

#define LIBNODE_NET_OPTION_MAP(GEN) \
    GEN(OPTION_ALLOW_HALF_OPEN, "allowHalfOpen") \
    GEN(OPTION_PORT,            "port") \
    GEN(OPTION_HOST,            "host") \
    GEN(OPTION_LOCAL_ADDRESS,   "localAddress") \
    GEN(OPTION_PATH,            "path")

#define LIBNODE_NET_OPTION_DECL_GEN(NAME, VAL) \
    extern Symbol::CPtr NAME;

LIBNODE_NET_OPTION_MAP(LIBNODE_NET_OPTION_DECL_GEN)

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_OPTION_H_
