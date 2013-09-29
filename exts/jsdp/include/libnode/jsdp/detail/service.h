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
        String::CPtr version,
        String::CPtr passwd = String::null())
        : port_(port)
        , name_(name)
        , version_(version)
        , password_(passwd) {
        LIBJ_STATIC_SYMBOL_DEF(symName,    "name");
        LIBJ_STATIC_SYMBOL_DEF(symPort,    "port");
        LIBJ_STATIC_SYMBOL_DEF(symVersion, "version");

        this->put(symName, name);
        this->put(symPort, port);
        if (version) this->put(symVersion, version);
    }

    virtual Int port() const {
        return port_;
    }

    virtual String::CPtr name() const {
        return name_;
    }

    virtual String::CPtr version() const {
        return version_;
    }

    virtual String::CPtr password() const {
        return password_;
    }

 private:
    Int port_;
    String::CPtr name_;
    String::CPtr version_;
    String::CPtr password_;
};

}  // namespace detail
}  // namespace jsdp
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSDP_DETAIL_SERVICE_H_
