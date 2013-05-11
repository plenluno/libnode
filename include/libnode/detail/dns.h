// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_DNS_H_
#define LIBNODE_DETAIL_DNS_H_

#include <libnode/dns.h>
#include <libnode/net.h>
#include <libnode/process.h>
#include <libnode/uv/error.h>
#include <libnode/detail/uv/req.h>

#include <uv.h>
#include <string.h>

namespace libj {
namespace node {
namespace detail {
namespace dns {

typedef class uv::Req<uv_getaddrinfo_t> GetAddrInfoReq;

static inline void setLastError() {
    node::uv::Error::setLast(uv_last_error(uv_default_loop()).code);
}

static void afterGetAddrInfo(
    uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
    GetAddrInfoReq* get = static_cast<GetAddrInfoReq*>(req->data);
    assert(get);

    JsArray::Ptr args = JsArray::create();

    if (status) {
        setLastError();
        args->add(JsArray::null());
    } else {
        char ip[INET6_ADDRSTRLEN];
        JsArray::Ptr results = JsArray::create();

        struct addrinfo* address = res;
        while (address) {
            assert(address->ai_socktype == SOCK_STREAM);

            if (address->ai_family == AF_INET) {
                struct sockaddr_in* in =
                    reinterpret_cast<struct sockaddr_in*>(address->ai_addr);
                const char* addr = reinterpret_cast<char*>(&(in->sin_addr));
                uv_err_t err = uv_inet_ntop(
                    address->ai_family,
                    addr,
                    ip,
                    INET6_ADDRSTRLEN);
                if (err.code == UV_OK) {
                    results->add(String::create(ip));
                }
            }
            address = address->ai_next;
        }

        address = res;
        while (address) {
            assert(address->ai_socktype == SOCK_STREAM);

            if (address->ai_family == AF_INET6) {
                struct sockaddr_in6* in =
                    reinterpret_cast<struct sockaddr_in6*>(address->ai_addr);
                const char* addr = reinterpret_cast<char*>(&(in->sin6_addr));
                uv_err_t err = uv_inet_ntop(
                    address->ai_family,
                    addr,
                    ip,
                    INET6_ADDRSTRLEN);
                if (err.code == UV_OK) {
                    results->add(String::create(ip));
                }
            }
            address = address->ai_next;
        }

        args->add(results);
    }

    uv_freeaddrinfo(res);
    (*(get->onComplete))(args);
    delete get;
}

static GetAddrInfoReq* getAddrInfo(String::CPtr domain, int family) {
    assert(domain);

    GetAddrInfoReq* get = new GetAddrInfoReq();

    struct addrinfo info;
    memset(&info, 0, sizeof(struct addrinfo));
    info.ai_family = family;
    info.ai_socktype = SOCK_STREAM;

    int r = uv_getaddrinfo(
        uv_default_loop(),
        &get->req,
        afterGetAddrInfo,
        domain->toStdString().c_str(),
        NULL,
        &info);
    get->dispatched();

    if (r) {
        setLastError();
        delete get;
        return NULL;
    } else {
        return get;
    }
}

class Apply : LIBJ_JS_FUNCTION(Apply)
 public:
    Apply(
        JsFunction::Ptr cb,
        JsArray::Ptr args)
        : callback_(cb)
        , args_(args) {}

    virtual Value operator()(JsArray::Ptr args) {
        return (*callback_)(args_);
    }

 private:
    JsFunction::Ptr callback_;
    JsArray::Ptr args_;
};

class OnAnswer : LIBJ_JS_FUNCTION(OnAnswer)
 public:
    OnAnswer(
        JsFunction::Ptr cb,
        Int family)
        : callback_(cb)
        , family_(family) {}

    virtual Value operator()(JsArray::Ptr args) {
        JsArray::CPtr addrs = args->getCPtr<JsArray>(0);
        if (addrs) {
            String::CPtr addr = addrs->getCPtr<String>(0);
            assert(addr);
            if (family_) {
                return callback_->call(
                    Error::null(),
                    addr,
                    family_);
            } else {
                return callback_->call(
                    Error::null(),
                    addr,
                    addr->charAt(0) == ':' ? 6 : 4);
            }
        } else {
            return callback_->call(node::uv::Error::last());
        }
    }

 private:
    JsFunction::Ptr callback_;
    Int family_;
};

Boolean lookup(String::CPtr domain, Int family, JsFunction::Ptr callback) {
    int fam;
    switch (family) {
    case 0:
        fam = AF_UNSPEC;
        break;
    case 4:
        fam = AF_INET;
        break;
    case 6:
        fam = AF_INET6;
        break;
    default:
        return false;
    }

    if (!domain) {
        JsArray::Ptr args = JsArray::create();
        args->add(Error::null());
        args->add(String::null());
        args->add(family == 6 ? 6 : 4);
        JsFunction::Ptr apply(new Apply(callback, args));
        process::nextTick(apply);
        return true;
    }

    Int ipv = net::isIP(domain);
    if (ipv) {
        JsArray::Ptr args = JsArray::create();
        args->add(Error::null());
        args->add(domain);
        args->add(ipv);
        JsFunction::Ptr apply(new Apply(callback, args));
        process::nextTick(apply);
        return true;
    }

    GetAddrInfoReq* req = getAddrInfo(domain, fam);
    if (!req) return false;

    JsFunction::Ptr onAnswer(new OnAnswer(callback, family));
    req->onComplete = onAnswer;
    return true;
}

Boolean lookup(String::CPtr domain, JsFunction::Ptr callback) {
    return lookup(domain, 0, callback);
}

}  // namespace dns
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_DNS_H_
