// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_SERVICE_H_
#define LIBNODE_JSDP_SERVICE_H_

#include <libj/js_object.h>

namespace libj {
namespace node {
namespace jsdp {

class Service : LIBJ_JS_OBJECT(Service)
 public:
    static Ptr create(
        String::CPtr name,
        Int port,
        String::CPtr version = String::null(),
        String::CPtr passwd = String::null());

    virtual Int port() const = 0;

    virtual String::CPtr name() const = 0;

    virtual String::CPtr version() const = 0;

    virtual String::CPtr password() const = 0;
};

}  // namespace jsdp
}  // namespace node
}  // namespace libj

#include <libnode/jsdp/impl/service.h>

#define LIBNODE_JSDP_SERVICE(T) \
    public libj::node::jsdp::Service { \
    LIBJ_MUTABLE_DEFS(T, LIBNODE_JSDP_SERVICE)

#endif  // LIBNODE_JSDP_SERVICE_H_
