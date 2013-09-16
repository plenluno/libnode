// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_OS_H_
#define LIBNODE_DETAIL_OS_H_

#include <libj/symbol.h>
#include <libj/js_array.h>
#include <libj/js_object.h>

#include <uv.h>

namespace libj {
namespace node {
namespace detail {
namespace os {

JsObject::Ptr networkInterface() {
    LIBJ_STATIC_SYMBOL_DEF(symIPv4,     "IPv4");
    LIBJ_STATIC_SYMBOL_DEF(symIPv6,     "IPv6");
    LIBJ_STATIC_SYMBOL_DEF(symFamily,   "family");
    LIBJ_STATIC_SYMBOL_DEF(symAddress,  "address");
    LIBJ_STATIC_SYMBOL_DEF(symInternal, "internal");

    uv_interface_address_t* interfaces;
    int count;
    uv_err_t err = uv_interface_addresses(&interfaces, &count);
    if (err.code) return JsObject::null();

    JsObject::Ptr res = JsObject::create();

    for (int i = 0; i < count; i++) {
        JsArray::Ptr ifs;
        String::CPtr name = String::create(interfaces[i].name);
        if (res->containsKey(name)) {
            ifs = res->getPtr<JsArray>(name);
        } else {
            ifs = JsArray::create();
            res->put(name, ifs);
        }

        String::CPtr family;
        char ip[INET6_ADDRSTRLEN];
        if (interfaces[i].address.address4.sin_family == AF_INET) {
            uv_ip4_name(
                &interfaces[i].address.address4,
                ip,
                sizeof(ip));
            family = symIPv4;
        } else if (interfaces[i].address.address4.sin_family == AF_INET6) {
            uv_ip6_name(
                &interfaces[i].address.address6,
                ip,
                sizeof(ip));
            family = symIPv6;
        } else {
            assert(false);
            family = String::null();
        }

        JsObject::Ptr ifo = JsObject::create();
        ifo->put(symFamily,  family);
        ifo->put(symAddress, String::create(ip));
        ifo->put(symInternal, !!interfaces[i].is_internal);
        ifs->add(ifo);
    }

    uv_free_interface_addresses(interfaces, count);
    return res;
}

}  // namespace os
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_OS_H_
