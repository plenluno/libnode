// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_
#define LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_

#include <libnode/os.h>
#include <libnode/util.h>
#include <libnode/dgram.h>
#include <libnode/timer.h>
#include <libnode/crypto.h>
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
        , callbacks_(libj::JsObject::create())
        , passwords_(libj::JsObject::create())
        , hash2passwd_(libj::JsObject::create()) {
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

        return removeService(service);
    }

    virtual void discover(JsFunction::Ptr cb) {
        discover(String::null(), String::null(), cb);
    }

    virtual void discover(String::CPtr name, JsFunction::Ptr cb) {
        discover(name, String::null(), cb);
    }

    virtual void discover(
        String::CPtr name,
        String::CPtr passwd,
        JsFunction::Ptr cb) {
        LIBJ_STATIC_SYMBOL_DEF(symId,       "id");
        LIBJ_STATIC_SYMBOL_DEF(symMethod,   "method");
        LIBJ_STATIC_SYMBOL_DEF(symParams,   "params");
        LIBJ_STATIC_SYMBOL_DEF(symDiscover, "discover");

        if (!cb) return;

        JsArray::Ptr params = JsArray::create();
        params->add(name);
        params->add(getHash(passwd, false));

        libj::JsObject::Ptr req = libj::JsObject::create();
        req->put(symMethod, symDiscover);
        req->put(symParams, params);
        req->put(symId, ++id_);

        passwords_->put(id_, passwd);
        callbacks_->put(id_, cb);
        setTimeout(JsFunction::Ptr(new OnTimeout(this, id_)), timeout_);

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
            Value result = msg->get(symResult);
            if (method && params && id) {
                if (method->equals(symDiscover)) {
                    discover(params, id, rinfo);
                }
            } else if (!result.isUndefined() && id) {
                handleResult(result, id, rinfo);
            }
            return UNDEFINED;
        }

     private:
        void discover(
            JsArray::Ptr params,
            Long id,
            libj::JsObject::CPtr rinfo) {
            LIBJ_STATIC_SYMBOL_DEF(symId,       "id");
            LIBJ_STATIC_SYMBOL_DEF(symResult,   "result");
            LIBJ_STATIC_SYMBOL_DEF(symName,     "name");
            LIBJ_STATIC_SYMBOL_DEF(symHost,     "host");
            LIBJ_STATIC_SYMBOL_DEF(symAddress,  "address");
            LIBJ_STATIC_SYMBOL_DEF(symServices, "services");

            String::CPtr name = params->getCPtr<String>(0);
            String::CPtr hash = params->getCPtr<String>(1);

            libj::JsObject::Ptr host = libj::JsObject::create();
            host->put(symName, os::hostname());

            libj::JsObject::Ptr result = libj::JsObject::create();
            result->put(symHost, host);
            result->put(symServices, self_->getServices(name, hash));

            libj::JsObject::Ptr response = libj::JsObject::create();
            response->put(symId, id);

            if (hash) {
                String::CPtr pwd = self_->hash2passwd_->getCPtr<String>(hash);
                if (!pwd) return;

                crypto::Cipher::Ptr cipher = crypto::createCipher(
                    crypto::Cipher::AES_256_CBC,
                    Buffer::create(pwd));
                Buffer::CPtr buf = Buffer::create(json::stringify(result));
                buf = cipher->update(buf);
                buf = buf->concat(cipher->final());
                response->put(symResult, buf->toString(Buffer::BASE64));
            } else {
                response->put(symResult, result);
            }

            String::CPtr addr = rinfo->getCPtr<String>(symAddress);
            self_->send(addr, response);
        }

        void handleResult(
            const Value& result,
            Long id,
            libj::JsObject::CPtr rinfo) {
            LIBJ_STATIC_SYMBOL_DEF(symHost,     "host");
            LIBJ_STATIC_SYMBOL_DEF(symAddress,  "address");

            String::CPtr pwd = self_->passwords_->getCPtr<String>(id);
            libj::JsObject::Ptr res = libj::JsObject::null();
            if (pwd) {
                String::CPtr r = toCPtr<String>(result);
                if (!r) return;

                crypto::Decipher::Ptr dechipher = crypto::createDecipher(
                    crypto::Cipher::AES_256_CBC,
                    Buffer::create(pwd));
                Buffer::CPtr buf = Buffer::create(r, Buffer::BASE64);
                buf = dechipher->update(buf);
                if (buf) {
                    buf = buf->concat(dechipher->final());
                }
                if (buf) {
                    res = toPtr<libj::JsObject>(json::parse(buf->toString()));
                }
            } else {
                res = toPtr<libj::JsObject>(result);
            }
            if (!res) return;

            libj::JsObject::Ptr host = res->getPtr<libj::JsObject>(symHost);
            if (host) {
                host->put(symAddress, rinfo->getCPtr<String>(symAddress));
            } else {
                return;
            }

            JsFunction::Ptr cb = self_->callbacks_->getPtr<JsFunction>(id);
            if (cb) cb->call(res);
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
            self_->passwords_->remove(id_);
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
    String::CPtr getHash(String::CPtr passwd, Boolean save = true) const {
        if (!passwd) return String::null();

        crypto::Hash::Ptr hash = createHash(crypto::Hash::SHA1);
        hash->update(Buffer::create(passwd));
        String::CPtr res = hash->digest()->toString(Buffer::BASE64);
        if (save) hash2passwd_->put(res, passwd);
        return res;
    }

    String::CPtr getKey(jsdp::Service::CPtr service) const {
        if (!service) return String::null();

        String::CPtr name = service->name();
        String::CPtr version = service->version();
        String::CPtr passwd = service->password();
        assert(name);

        StringBuilder::Ptr key = StringBuilder::create();
        key->append(name);
        key->appendChar(':');
        if (version) key->append(version);
        key->appendChar(':');
        if (passwd) key->append(getHash(passwd));
        return key->toString();
    }

    Boolean putService(jsdp::Service::CPtr service) {
        if (service) {
            services_->put(getKey(service), service);
            return true;
        } else {
            return false;
        }
    }

    Boolean removeService(jsdp::Service::CPtr service) {
        if (service) {
            String::CPtr key = getKey(service);
            if (services_->containsKey(key)) {
                services_->remove(key);
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    Boolean startsWith(String::CPtr key, String::CPtr name) const {
        if (name) {
            assert(key);
            return key->startsWith(name) && key->charAt(name->length()) == ':';
        } else {
            return true;
        }
    }

    Boolean endsWith(String::CPtr key, String::CPtr hash) const {
        assert(key->length());
        if (key->charAt(key->length() - 1) == ':') {
            return true;
        } else {
            assert(key);
            return hash &&
                key->endsWith(hash) &&
                key->length() > hash->length() + 1 &&
                key->charAt(key->length() - hash->length() - 1) == ':';
        }
    }

    JsArray::Ptr getServices(String::CPtr name, String::CPtr hash) const {
        JsArray::Ptr res = JsArray::create();
        TypedSet<libj::Map::Entry::CPtr>::CPtr es = services_->entrySet();
        TypedIterator<libj::Map::Entry::CPtr>::Ptr itr = es->iteratorTyped();
        while (itr->hasNext()) {
            libj::Map::Entry::CPtr e = itr->nextTyped();
            String::CPtr key = toCPtr<String>(e->getKey());
            if (startsWith(key, name) && endsWith(key, hash)) {
                res->add(e->getValue());
            }
        }
        return res;
    }

    void send(String::CPtr addr, libj::JsObject::CPtr obj) {
        Buffer::Ptr buf = Buffer::create(json::stringify(obj));
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
    libj::JsObject::Ptr passwords_;
    libj::JsObject::Ptr hash2passwd_;
};

}  // namespace detail
}  // namespace jsdp
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSDP_DETAIL_DISCOVERY_SERVICE_H_
