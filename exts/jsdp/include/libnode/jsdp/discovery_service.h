// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_DISCOVERY_SERVICE_H_
#define LIBNODE_JSDP_DISCOVERY_SERVICE_H_

#include <libnode/jsdp/service.h>

#include <libj/js_function.h>

namespace libj {
namespace node {
namespace jsdp {

class DiscoveryService : LIBNODE_JSDP_SERVICE(DiscoveryService)
    static Ptr create(
        String::CPtr multicastAddr,
        Int port,
        UInt timeout = 5000);

    virtual void close() = 0;

    virtual Boolean join(Service::CPtr service) = 0;

    virtual Boolean leave(Service::CPtr service) = 0;

    virtual void discover(JsFunction::Ptr cb) = 0;

    virtual void discover(String::CPtr name, JsFunction::Ptr cb) = 0;

    virtual void discover(
        String::CPtr name, String::CPtr passwd, JsFunction::Ptr cb) = 0;
};

}  // namespace jsdp
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSDP_DISCOVERY_SERVICE_H_
