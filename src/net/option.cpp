// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/net/option.h>

namespace libj {
namespace node {
namespace net {

#define LIBNODE_NET_OPTION_DEF_GEN(NAME, VAL) \
    LIBJ_SYMBOL_DEF(NAME, VAL)

LIBNODE_NET_OPTION_MAP(LIBNODE_NET_OPTION_DEF_GEN)

}  // namespace net
}  // namespace node
}  // namespace libj
