// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/net.h>
#include <libnode/jsdp.h>
#include <libnode/jsdp/detail/service.h>
#include <libnode/jsdp/detail/discovery_service.h>

namespace libj {
namespace node {
namespace jsdp {

Service::Ptr Service::create(
    String::CPtr name,
    Int port,
    String::CPtr version,
    String::CPtr passwd) {
    if (name && port > 0) {
        return Service::Ptr(
            new jsdp::detail::Service<Service>(name, port, version, passwd));
    } else {
        return Service::null();
    }
}

DiscoveryService::Ptr DiscoveryService::create(
    String::CPtr multicastAddr,
    Int port,
    UInt timeout) {
    LIBJ_STATIC_CONST_STRING_DEF(str239, "239.");

    if (net::isIPv4(multicastAddr) &&
        multicastAddr->startsWith(str239) &&
        port > 0) {
        return DiscoveryService::Ptr(
            new jsdp::detail::DiscoveryService(multicastAddr, port, timeout));
    } else {
        return DiscoveryService::null();
    }
}

}  // namespace jsdp
}  // namespace node
}  // namespace libj
