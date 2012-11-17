// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_AGENT_IMPL_H_
#define LIBNODE_SRC_HTTP_AGENT_IMPL_H_


#include <libj/string_buffer.h>

#include "libnode/http/agent.h"
#include "libnode/net.h"
#include "libnode/util.h"

#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

class AgentImpl : public Agent {
 public:
    typedef LIBJ_PTR(AgentImpl) Ptr;
    typedef LIBJ_CPTR(AgentImpl) CPtr;

    static Ptr create(JsObject::CPtr options) {
        LIBJ_STATIC_SYMBOL_DEF(strMaxSocks, "maxSockets");

        AgentImpl* agent = new AgentImpl();
        if (options) {
            agent->options_ = options;
            to<Size>(options->get(strMaxSocks), &agent->maxSockets_);
        }
        return Ptr(agent);
    }

    void addRequest(
        OutgoingMessage::Ptr req,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress) {
        StringBuffer::Ptr sb = StringBuffer::create();
        sb->append(host);
        sb->appendChar(':');
        sb->append(port);
        if (localAddress) {
            sb->appendChar(':');
            sb->append(localAddress);
        }
        String::CPtr name = sb->toString();

        JsArray::Ptr ss = sockets_->getPtr<JsArray>(name);
        if (!ss) {
            ss = JsArray::create();
            sockets_->put(name, ss);
        }
        if (ss->length() < maxSockets_) {
            req->onSocket(createSocket(name, host, port, localAddress, req));
        } else {
            JsArray::Ptr rs = requests_->getPtr<JsArray>(name);
            if (!rs) {
                rs = JsArray::create();
                requests_->put(name, rs);
            }
            rs->push(req);
        }
    }

    net::SocketImpl::Ptr createSocket(
        String::CPtr name,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress,
        OutgoingMessage::Ptr req) {
        LIBJ_STATIC_SYMBOL_DEF(strPort, "port");
        LIBJ_STATIC_SYMBOL_DEF(strHost, "host");
        LIBJ_STATIC_SYMBOL_DEF(strLocalAddress, "localAddress");
        LIBJ_STATIC_SYMBOL_DEF(strServerName, "servername");

        JsObject::Ptr options = JsObject::create();
        util::extend(options, options_);
        options->put(strPort, port);
        options->put(strHost, host);
        options->put(strLocalAddress, localAddress);
        options->put(strServerName, host);

        if (req) {
            String::CPtr hostHeader = req->getHeader(strHost);
            if (hostHeader) {
                Size index = hostHeader->lastIndexOf(':');
                options->put(strServerName, host->substring(0, index));
            }
        }

        net::SocketImpl::Ptr socket = net::SocketImpl::create(options);
        socket->connect(options);
        JsArray::Ptr ss = sockets_->getPtr<JsArray>(name);
        if (!ss) {
            ss = JsArray::create();
            sockets_->put(name, ss);
        }
        ss->push(socket);

        OnFree::Ptr onFree =
            OnFree::create(this, socket, host, port, localAddress);
        OnClose::Ptr onClose =
            OnClose::create(this, socket, name, host, port, localAddress);
        OnRemove::Ptr onRemove = OnRemove::create(
            this, socket, name, host, port, localAddress, onFree, onClose);
        socket->on(EVENT_FREE, onFree);
        socket->on(EVENT_CLOSE, onClose);
        socket->on(EVENT_AGENT_REMOVE, onRemove);
        return socket;
    }

    void removeSocket(
        net::SocketImpl::Ptr socket,
        String::CPtr name,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress) {
        JsArray::Ptr ss = socket->getPtr<JsArray>(name);
        if (ss) {
            ss->remove(socket);
            if (ss->isEmpty()) {
                socket->remove(name);
            }
        }

        JsArray::Ptr rs = requests_->getPtr<JsArray>(name);
        if (rs && !rs->isEmpty()) {
            OutgoingMessage::Ptr req = requests_->getPtr<OutgoingMessage>(0);
            createSocket(name, host, port, localAddress, req)->emit(EVENT_FREE);
        }
    }

 private:
    class Free : LIBJ_JS_FUNCTION(Free)
        static Ptr create(JsObject::Ptr reqs) {
            return Ptr(new Free(reqs));
        }

        Value operator()(JsArray::Ptr args) {
            net::SocketImpl::Ptr socket = args->getPtr<net::SocketImpl>(0);
            String::CPtr host = args->getCPtr<String>(1);
            String::CPtr port = args->getCPtr<String>(2);
            String::CPtr localAddress = args->getCPtr<String>(3);

            StringBuffer::Ptr sb = StringBuffer::create();
            sb->append(host);
            sb->appendChar(':');
            sb->append(port);
            if (localAddress) {
                sb->appendChar(':');
                sb->append(localAddress);
            }
            String::CPtr name = sb->toString();

            JsArray::Ptr rs = requests_->getPtr<JsArray>(name);
            if (rs && rs->length()) {
                OutgoingMessage::Ptr req = toPtr<OutgoingMessage>(rs->shift());
                req->onSocket(socket);
                if (rs->isEmpty()) {
                    requests_->remove(name);
                }
            } else {
                socket->destroy();
            }
            return libj::Status::OK;
        }

     private:
        JsObject::Ptr requests_;

        Free(JsObject::Ptr reqs) : requests_(reqs) {}
    };

    class OnFree : LIBJ_JS_FUNCTION(OnFree)
        static Ptr create(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress) {
            return Ptr(new OnFree(self, socket, host, port, localAddress));
        }

        Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_FREE, socket_, host_, port_, localAddress_);
            return libj::Status::OK;
        }

     private:
        AgentImpl* self_;
        net::SocketImpl::Ptr socket_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;

        OnFree(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress)
            : self_(self)
            , socket_(socket)
            , host_(host)
            , localAddress_(localAddress) {}
    };

    class OnClose : LIBJ_JS_FUNCTION(OnClose)
        static Ptr create(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr name,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress) {
            return Ptr(new OnClose(
                self, socket, name, host, port, localAddress));
        }

        Value operator()(JsArray::Ptr args) {
            self_->removeSocket(socket_, name_, host_, port_, localAddress_);
            return libj::Status::OK;
        }

     private:
        AgentImpl* self_;
        net::SocketImpl::Ptr socket_;
        String::CPtr name_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;

        OnClose(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr name,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress)
            : self_(self)
            , socket_(socket)
            , name_(name)
            , host_(host)
            , localAddress_(localAddress) {}
    };

    class OnRemove : LIBJ_JS_FUNCTION(OnRemove)
        static Ptr create(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr name,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress,
            OnFree::Ptr onFree,
            OnClose::Ptr onClose) {
            return Ptr(new OnRemove(
                self, socket, name, host, port, localAddress,
                onFree, onClose));
        }

        Value operator()(JsArray::Ptr args) {
            self_->removeSocket(socket_, name_, host_, port_, localAddress_);
            socket_->removeListener(EVENT_FREE, onFree_);
            socket_->removeListener(EVENT_CLOSE, onClose_);
            socket_->removeAllListeners(EVENT_AGENT_REMOVE);
            return libj::Status::OK;
        }

     private:
        AgentImpl* self_;
        net::SocketImpl::Ptr socket_;
        String::CPtr name_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;
        OnFree::Ptr onFree_;
        OnClose::Ptr onClose_;

        OnRemove(
            AgentImpl* self,
            net::SocketImpl::Ptr socket,
            String::CPtr name,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress,
            OnFree::Ptr onFree,
            OnClose::Ptr onClose)
            : self_(self)
            , socket_(socket)
            , name_(name)
            , host_(host)
            , localAddress_(localAddress)
            , onFree_(onFree)
            , onClose_(onClose) {}
    };

 private:
    static Symbol::CPtr EVENT_CLOSE;
    static Symbol::CPtr EVENT_FREE;
    static Symbol::CPtr EVENT_AGENT_REMOVE;

    static const UInt defaultPort_;
    static const Size defaultMaxSockets_;

    Size maxSockets_;
    JsObject::Ptr requests_;
    JsObject::Ptr sockets_;
    JsObject::CPtr options_;
    events::EventEmitter::Ptr ee_;

    AgentImpl()
        : maxSockets_(defaultMaxSockets_)
        , requests_(JsObject::create())
        , sockets_(JsObject::create())
        , options_(JsObject::create())
        , ee_(events::EventEmitter::create()) {
        on(EVENT_FREE, Free::create(requests_));
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_AGENT_IMPL_H_
