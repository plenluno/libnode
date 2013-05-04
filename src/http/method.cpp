// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/http/method.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_METHOD_DEF_GEN(NAME, VAL) \
    LIBJ_SYMBOL_DEF(NAME, VAL)

LIBNODE_HTTP_METHOD_MAP(LIBNODE_HTTP_METHOD_DEF_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj
