// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_AGENT_H_
#define LIBNODE_DETAIL_HTTP_AGENT_H_

#include <libnode/http/agent.h>
#include <libnode/util.h>
#include <libnode/detail/net/socket.h>
#include <libnode/detail/http/outgoing_message.h>

#include <libj/this.h>
#include <libj/string_buffer.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

class Agent : public events::EventEmitter<node::http::Agent> {
 public:
    LIBJ_MUTABLE_DEFS(Agent, node::http::Agent);

    static Ptr create(libj::JsObject::CPtr options) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE,         "free");
        LIBJ_STATIC_SYMBOL_DEF(OPTION_MAX_SOCKETS, "maxSockets");

        Agent* agent = new Agent();
        if (options) {
            agent->options_ = options;
            to<Size>(options->get(OPTION_MAX_SOCKETS), &agent->maxSockets_);
        }

        agent->on(EVENT_FREE, JsFunction::Ptr(new Free(agent->requests_)));
        return Ptr(agent);
    }

    virtual Size maxSockets() const {
        return maxSockets_;
    }

    virtual void setMaxSockets(Size max) {
        maxSockets_ = max;
    }

    void addRequest(
        OutgoingMessage::Ptr req,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress) {
        LIBJ_STATIC_SYMBOL_DEF(symRequest, "$request");

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
            net::Socket::Ptr socket =
                createSocket(name, host, port, localAddress, req);
            req->onSocket(socket);
            socket->put(symRequest, req);
        } else {
            JsArray::Ptr rs = requests_->getPtr<JsArray>(name);
            if (!rs) {
                rs = JsArray::create();
                requests_->put(name, rs);
            }
            rs->push(req);
        }
    }

    net::Socket::Ptr createSocket(
        String::CPtr name,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress,
        OutgoingMessage::Ptr req) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE,         "free");
        LIBJ_STATIC_SYMBOL_DEF(EVENT_CLOSE,        "close");
        LIBJ_STATIC_SYMBOL_DEF(EVENT_AGENT_REMOVE, "agentRemove");
        LIBJ_STATIC_SYMBOL_DEF(OPTION_SERVERNAME,  "servername");

        libj::JsObject::Ptr options = libj::JsObject::create();
        util::extend(options, options_);
        options->put(node::net::OPTION_PORT, port);
        options->put(node::net::OPTION_HOST, host);
        options->put(node::net::OPTION_LOCAL_ADDRESS, localAddress);
        options->put(OPTION_SERVERNAME, host);

        if (req) {
            String::CPtr hostHeader = req->getHeader(node::http::LHEADER_HOST);
            if (hostHeader) {
                Size index = hostHeader->lastIndexOf(':');
                options->put(OPTION_SERVERNAME, host->substring(0, index));
            }
        }

        net::Socket::Ptr socket = net::Socket::create(options);
        socket->connect(options);
        JsArray::Ptr ss = sockets_->getPtr<JsArray>(name);
        if (!ss) {
            ss = JsArray::create();
            sockets_->put(name, ss);
        }
        ss->push(socket);

        OnFree::Ptr onFree(new OnFree(
            this, socket, host, port, localAddress));
        socket->on(EVENT_FREE, onFree);

        OnClose::Ptr onClose(new OnClose(
            this, socket, name, host, port, localAddress));
        socket->on(EVENT_CLOSE, onClose);

        OnRemove::Ptr onRemove(new OnRemove(
            this, socket, name, host, port, localAddress, onFree, onClose));
        socket->on(EVENT_AGENT_REMOVE, onRemove);
        return socket;
    }

    void removeSocket(
        net::Socket::Ptr socket,
        String::CPtr name,
        String::CPtr host,
        String::CPtr port,
        String::CPtr localAddress) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE, "free");

        JsArray::Ptr ss = sockets_->getPtr<JsArray>(name);
        if (ss) {
            ss->remove(socket);
            if (ss->isEmpty()) {
                sockets_->remove(name);
            }
        }

        JsArray::Ptr rs = requests_->getPtr<JsArray>(name);
        if (rs && rs->length()) {
            OutgoingMessage::Ptr req = rs->getPtr<OutgoingMessage>(0);
            createSocket(name, host, port, localAddress, req)->emit(EVENT_FREE);
        }
    }

 private:
    class Free : LIBJ_JS_FUNCTION(Free)
     public:
        Free(libj::JsObject::Ptr reqs) : requests_(reqs) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(symRequest, "$request");

            net::Socket::Ptr socket = args->getPtr<net::Socket>(0);
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
                socket->put(symRequest, req);

                if (rs->isEmpty()) {
                    requests_->remove(name);
                }
            } else {
                socket->destroy();
                socket->remove(symRequest);
            }
            return Status::OK;
        }

     private:
        libj::JsObject::Ptr requests_;
    };

    class OnFree : LIBJ_JS_FUNCTION(OnFree)
     public:
        OnFree(
            Agent* self,
            net::Socket::Ptr socket,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress)
            : self_(self)
            , socket_(socket)
            , host_(host)
            , port_(port)
            , localAddress_(localAddress) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE, "free");

            self_->emit(EVENT_FREE, socket_, host_, port_, localAddress_);
            return Status::OK;
        }

     private:
        Agent* self_;
        net::Socket::Ptr socket_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;
    };

    class OnClose : LIBJ_JS_FUNCTION(OnClose)
     public:
        OnClose(
            Agent* self,
            net::Socket::Ptr socket,
            String::CPtr name,
            String::CPtr host,
            String::CPtr port,
            String::CPtr localAddress)
            : self_(self)
            , socket_(socket)
            , name_(name)
            , host_(host)
            , port_(port)
            , localAddress_(localAddress) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->removeSocket(socket_, name_, host_, port_, localAddress_);
            process::nextTick(JsFunction::Ptr(new SocketFree(socket_)));
            return Status::OK;
        }

     private:
        Agent* self_;
        net::Socket::Ptr socket_;
        String::CPtr name_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;
    };

    class OnRemove : LIBJ_JS_FUNCTION(OnRemove)
        OnRemove(
            Agent* self,
            net::Socket::Ptr socket,
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
            , port_(port)
            , localAddress_(localAddress)
            , onFree_(onFree)
            , onClose_(onClose) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE,         "free");
            LIBJ_STATIC_SYMBOL_DEF(EVENT_CLOSE,        "close");
            LIBJ_STATIC_SYMBOL_DEF(EVENT_AGENT_REMOVE, "agentRemove");

            self_->removeSocket(socket_, name_, host_, port_, localAddress_);
            socket_->removeListener(EVENT_FREE, onFree_);
            socket_->removeListener(EVENT_CLOSE, onClose_);
            socket_->removeListener(
                EVENT_AGENT_REMOVE, LIBJ_THIS_PTR(OnRemove));
            return Status::OK;
        }

     private:
        Agent* self_;
        net::Socket::Ptr socket_;
        String::CPtr name_;
        String::CPtr host_;
        String::CPtr port_;
        String::CPtr localAddress_;
        OnFree::Ptr onFree_;
        OnClose::Ptr onClose_;
    };

    class SocketFree : LIBJ_JS_FUNCTION(SocketFree)
     public:
         SocketFree(net::Socket::Ptr socket)
            : socket_(socket) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(symRequest, "$request");

            socket_->remove(symRequest);

            Parser* parser = socket_->parser();
            if (parser) {
                delete parser;
                socket_->setParser(NULL);
            }
            return Status::OK;
        }

     private:
        net::Socket::Ptr socket_;
    };

 private:
    Size maxSockets_;
    libj::JsObject::Ptr sockets_;
    libj::JsObject::Ptr requests_;
    libj::JsObject::CPtr options_;

    Agent()
        : maxSockets_(5)
        , sockets_(libj::JsObject::create())
        , requests_(libj::JsObject::create())
        , options_(libj::JsObject::create()) {}
};

#ifdef LIBNODE_DEBUG

inline Agent::Ptr globalAgent(Boolean free = false) {
    static Agent::Ptr global = Agent::create(JsObject::null());
    if (free) {
        global = Agent::null();
    }
    return global;
}

inline void freeGlobalAgent() {
    globalAgent(true);
}

#else

inline Agent::Ptr globalAgent() {
    static Agent::Ptr global = Agent::create(JsObject::null());
    return global;
}

#endif

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_AGENT_H_
