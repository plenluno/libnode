// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/http/header.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_HEADER_DEF_GEN(NAME, VAL) \
    LIBJ_SYMBOL_DEF(NAME, VAL)

#define LIBNODE_HTTP_LHEADER_DEF_GEN(NAME, VAL) \
    LIBJ_SYMBOL_DEF(L##NAME, String::create(VAL)->toLowerCase())

LIBNODE_HTTP_HEADER_MAP(LIBNODE_HTTP_HEADER_DEF_GEN)

LIBNODE_HTTP_HEADER_MAP(LIBNODE_HTTP_LHEADER_DEF_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj
