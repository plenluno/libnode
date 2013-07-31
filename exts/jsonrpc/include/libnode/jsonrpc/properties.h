// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_PROPERTIES_H_
#define LIBNODE_JSONRPC_PROPERTIES_H_

#include <libj/symbol.h>

namespace libj {
namespace node {
namespace jsonrpc {

extern Symbol::CPtr ID;
extern Symbol::CPtr METHOD;
extern Symbol::CPtr PARAMS;
extern Symbol::CPtr RESULT;
extern Symbol::CPtr ERROR;
extern Symbol::CPtr CODE;
extern Symbol::CPtr MESSAGE;
extern Symbol::CPtr JSONRPC;

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_PROPERTIES_H_
