// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/jsonrpc/service.h>
#include <libnode/jsonrpc/detail/service.h>

namespace libj {
namespace node {
namespace jsonrpc {

LIBJ_SYMBOL_DEF(Service::EVENT_RESPONSE, "response");

Service::Ptr Service::create() {
    return Service::Ptr(new jsonrpc::detail::Service());
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj
