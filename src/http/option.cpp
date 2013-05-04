// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/http/option.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_OPTION_DEF_GEN(NAME, VAL) \
    LIBJ_SYMBOL_DEF(NAME, VAL)

LIBNODE_HTTP_OPTION_MAP(LIBNODE_HTTP_OPTION_DEF_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj
