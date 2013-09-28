// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_
#define LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_

#include <libnode/os.h>
#include <libnode/util.h>
#include <libnode/dgram.h>
#include <libnode/timer.h>
#include <libnode/debug_print.h>
#include <libnode/jsdp/discovery_service.h>
#include <libnode/jsdp/detail/service.h>

#include <libj/this.h>
#include <libj/json.h>

namespace libj {
namespace node {
namespace jsdp {
namespace detail {

class DiscoveryService : public Service<jsdp::DiscoveryService> {
 public:
    DiscoveryService(
        String::CPtr multicastAddr,
        Int port,
        UInt timeout)
        : Service<jsdp::DiscoveryService>(
            String::intern("jsds"),
            port,
            String::intern("1.0"))
        , id_(0)
        , timeout_(timeout)
        , socket_(dgram::createSocket(dgram::Socket::UDP4))
        , multicastAddr_(multicastAddr)
        , services_(libj::JsObject::create())
        , callbacks_(libj::JsObject::create()) {
        socket_->on(
            dgram::Socket::EVENT_MESSAGE,
            JsFunction::Ptr(new OnMessage(this)));
        socket_->on(
            dgram::Socket::EVENT_CLOSE,
            JsFunction::Ptr(new OnClose(this)));
        socket_->bind(
            port,
            JsFunction::Ptr(new OnBind(this)));
    }

    virtual void close() {
        socket_->close();
    }

    virtual Boolean join(jsdp::Service::CPtr service) {
        LIBJ_STATIC_SYMBOL_DEF(symJsds, "jsds");

        if (!service) return false;

        String::CPtr name = service->name();
        assert(name);
        if (name->equals(symJsds)) return false;

        return putService(service);
    }

    virtual Boolean leave(jsdp::Service::CPtr service) {
        LIBJ_STATIC_SYMBOL_DEF(symJsds, "jsds");

        if (!service) return false;

        String::CPtr name = service->name();
        assert(name);
        if (name->equals(symJsds)) return false;

        libj::JsObject::Ptr obj = services_->getPtr<libj::JsObject>(name);
        String::CPtr version = service->version();
        if (obj && obj->containsKey(version)) {
            obj->remove(version);
            return true;
        } else {
            return false;
        }
    }

    virtual void discover(JsFunction::Ptr cb) {
        discover(String::null(), cb);
    }

    virtual void discover(String::CPtr name, JsFunction::Ptr cb) {
        LIBJ_STATIC_SYMBOL_DEF(symMethod,   "method");
        LIBJ_STATIC_SYMBOL_DEF(symDiscover, "discover");
        LIBJ_STATIC_SYMBOL_DEF(symParams,   "params");
        LIBJ_STATIC_SYMBOL_DEF(symId,       "id");

        JsArray::Ptr params = JsArray::create();
        if (name) params->add(name);

        libj::JsObject::Ptr req = libj::JsObject::create();
        req->put(symMethod, symDiscover);
        req->put(symParams, JsArray::create());
        req->put(symId, ++id_);

        if (cb) {
            callbacks_->put(id_, cb);
            setTimeout(JsFunction::Ptr(new OnTimeout(this, id_)), timeout_);
        }

        send(multicastAddr_, req);
    }

 private:
    class OnMessage : LIBJ_JS_FUNCTION(OnMessage)
     public:
        OnMessage(DiscoveryService* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(symId,       "id");
            LIBJ_STATIC_SYMBOL_DEF(symMethod,   "method");
            LIBJ_STATIC_SYMBOL_DEF(symParams,   "params");
            LIBJ_STATIC_SYMBOL_DEF(symResult,   "result");
            LIBJ_STATIC_SYMBOL_DEF(symHost,     "host");
            LIBJ_STATIC_SYMBOL_DEF(symName,     "name");
            LIBJ_STATIC_SYMBOL_DEF(symAddress,  "address");
            LIBJ_STATIC_SYMBOL_DEF(symServices, "services");
            LIBJ_STATIC_SYMBOL_DEF(symDiscover, "discover");

            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            libj::JsObject::CPtr rinfo = args->getCPtr<libj::JsObject>(1);
            if (!buf || !rinfo) return UNDEFINED;

            Value val = json::parse(buf->toString());
            libj::JsObject::Ptr msg = toPtr<libj::JsObject>(val);
            if (!msg) return UNDEFINED;

            Long id = to<Long>(msg->get(symId));
            String::CPtr method = msg->getCPtr<String>(symMethod);
            JsArray::Ptr params = msg->getPtr<JsArray>(symParams);
            libj::JsObject::Ptr result = msg->getPtr<libj::JsObject>(symResult);
            libj::JsObject::Ptr host;
            if (method && params && id && method->equals(symDiscover)) {
                host = libj::JsObject::create();
                host->put(symName, os::hostname());

                String::CPtr name;
                if (params->length() == 1) {
                    name = params->getCPtr<String>(0);
                } else {
                    name = String::null();
                }

                result = libj::JsObject::create();
                result->put(symHost, host);
                result->put(symServices, self_->getServices(name));

                libj::JsObject::Ptr res = libj::JsObject::create();
                res->put(symId, id);
                res->put(symResult, result);

                String::CPtr addr = rinfo->getCPtr<String>(symAddress);
                self_->send(addr, res);
            } else if (result && id) {
                host = result->getPtr<libj::JsObject>(symHost);
                assert(host);
                host->put(symAddress, rinfo->getCPtr<String>(symAddress));
                JsFunction::Ptr cb = self_->callbacks_->getPtr<JsFunction>(id);
                if (cb) cb->call(result);
            }
            return UNDEFINED;
        }

     private:
        DiscoveryService* self_;
    };

    class OnClose : LIBJ_JS_FUNCTION(OnClose)
     public:
        OnClose(DiscoveryService* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->services_->clear();
            return UNDEFINED;
        }

     private:
        DiscoveryService* self_;
    };

    class OnBind : LIBJ_JS_FUNCTION(OnBind)
     public:
        OnBind(DiscoveryService* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
#ifdef LIBNODE_DEBUG
            self_->socket_->setMulticastLoopback(true);
#else
            self_->socket_->setMulticastLoopback(false);
#endif
            self_->socket_->addMembership(self_->multicastAddr_);
            self_->putService(
                LIBJ_STATIC_PTR_CAST(DiscoveryService)(self_->self()));
            return UNDEFINED;
        }

     private:
        DiscoveryService* self_;
    };

    class OnTimeout : LIBJ_JS_FUNCTION(OnTimeout)
     public:
        OnTimeout(
            DiscoveryService* self,
            Long id)
            : self_(self)
            , id_(id) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->callbacks_->remove(id_);
            LIBNODE_DEBUG_PRINT("discovery timeout: %d", id_);
            return UNDEFINED;
        }

     private:
        DiscoveryService* self_;
        Long id_;
    };

    class AfterSend : LIBJ_JS_FUNCTION(AfterSend)
     public:
        virtual Value operator()(JsArray::Ptr args) {
            assert(!args->getCPtr<libj::Error>(0));
            return UNDEFINED;
        }
    };

 private:
    Boolean putService(jsdp::Service::CPtr service) {
        assert(service);
        String::CPtr name = service->name();
        libj::JsObject::Ptr obj = services_->getPtr<libj::JsObject>(name);
        if (!obj) {
            obj = libj::JsObject::create();
            services_->put(name, obj);
        }
        obj->put(service->version(), service);
        return true;
    }

    JsArray::Ptr getServices(String::CPtr name) const {
        JsArray::Ptr res = JsArray::create();
        if (name) {
            getServices(name, res);
        } else {
            typedef libj::Map::Entry ServiceEntry;
            TypedSet<ServiceEntry::CPtr>::CPtr es = services_->entrySet();
            TypedIterator<ServiceEntry::CPtr>::Ptr itr = es->iteratorTyped();
            while (itr->hasNext()) {
                getServices(toCPtr<String>(itr->nextTyped()->getKey()), res);
            }
        }
        return res;
    }

    void getServices(String::CPtr name, JsArray::Ptr result) const {
        assert(name  && result);
        libj::JsObject::Ptr obj = services_->getPtr<libj::JsObject>(name);
        if (!obj) return;

        TypedSet<libj::Map::Entry::CPtr>::CPtr es = obj->entrySet();
        TypedIterator<libj::Map::Entry::CPtr>::Ptr itr = es->iteratorTyped();
        while (itr->hasNext()) {
            result->add(itr->nextTyped()->getValue());
        }
    }

    void send(String::CPtr addr, libj::JsObject::Ptr msg) {
        Buffer::Ptr buf = Buffer::create(json::stringify(msg));
        socket_->send(
            buf,
            0,
            buf->length(),
            port(),
            addr,
#ifdef LIBNODE_DEBUG
            JsFunction::Ptr(new AfterSend()));
#else
            JsFunction::null());
#endif
    }

 private:
    Long id_;
    UInt timeout_;
    dgram::Socket::Ptr socket_;
    String::CPtr multicastAddr_;
    libj::JsObject::Ptr services_;
    libj::JsObject::Ptr callbacks_;
};

}  // namespace detail
}  // namespace jsdp
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_
