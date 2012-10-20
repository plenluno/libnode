// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http/header.h"

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_HEADER_DEF_GEN(NAME, VAL) \
    const String::CPtr NAME = String::intern(VAL);

#define LIBNODE_HTTP_LHEADER_DEF_GEN(NAME, VAL) \
    const String::CPtr L##NAME = String::intern(VAL)->toLowerCase();

LIBNODE_HTTP_HEADER_MAP(
    LIBNODE_HTTP_HEADER_DEF_GEN)

LIBNODE_HTTP_HEADER_MAP(
    LIBNODE_HTTP_LHEADER_DEF_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj
