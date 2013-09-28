// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_DETAIL_SERVICE_H_
#define LIBNODE_JSDP_DETAIL_SERVICE_H_

#include <libnode/jsdp/service.h>

#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace jsdp {
namespace detail {

template<typename I>
class Service : public libj::detail::JsObject<I> {
 public:
    Service(
        String::CPtr name,
        Int port,
        String::CPtr version = String::null())
        : name_(name)
        , version_(version)
        , port_(port) {
        LIBJ_STATIC_SYMBOL_DEF(symName,    "name");
        LIBJ_STATIC_SYMBOL_DEF(symPort,    "port");
        LIBJ_STATIC_SYMBOL_DEF(symVersion, "version");

        this->put(symName, name);
        this->put(symPort, port);
        if (version) this->put(symVersion, version);
    }

    virtual String::CPtr name() const {
        return name_;
    }

    virtual String::CPtr version() const {
        return version_;
    }

    virtual Int port() const {
        return port_;
    }

 private:
    String::CPtr name_;
    String::CPtr version_;
    Int port_;
};

}  // namespace detail
}  // namespace jsdp
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSDP_DETAIL_SERVICE_H_
