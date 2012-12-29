// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_NET_SERVER_H_
#define LIBNODE_DETAIL_NET_SERVER_H_

#include <libnode/detail/net/socket.h>

#include <libj/string_buffer.h>

namespace libj {
namespace node {
namespace detail {
namespace net {

template<typename I>
class Server : public events::EventEmitter<I> {
 public:
    LIBJ_MUTABLE_TEMPLATE_DEFS(Server, I);

    static Ptr create(
        libj::JsObject::CPtr options = libj::JsObject::null(),
        JsFunction::Ptr listener = JsFunction::null()) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_DESTROY, "destroy");

        Ptr server(new Server());

        if (options) {
            Boolean allowHalfOpen = false;
            to<Boolean>(
                options->get(node::net::OPTION_ALLOW_HALF_OPEN),
                &allowHalfOpen);
            if (allowHalfOpen) server->setFlag(ALLOW_HALF_OPEN);
        }

        if (listener) {
            server->on(node::net::Server::EVENT_CONNECTION, listener);
        }

        JsFunction::Ptr onDestroy(new OnDestroy(server));
        server->on(EVENT_DESTROY, onDestroy);
        return server;
    }

    virtual Value address() {
        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return tcp->getSockName();
        } else if (pipeName_) {
            return pipeName_;
        } else {
            return libj::JsObject::null();
        }
    }

    virtual Size connections() const {
        return connections_;
    }

    virtual Size maxConnections() const {
        return maxConnections_;
    }

    virtual void setMaxConnections(Size max) {
        maxConnections_ = max;
    }

    virtual Boolean listen(
        Int port,
        String::CPtr host = node::net::Server::IN_ADDR_ANY,
        Int backlog = 511,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (handle_) return false;

        if (callback) this->once(node::net::Server::EVENT_LISTENING, callback);

        Int addressType = node::net::isIP(host);
        if (addressType) {
            return listen(host, port, addressType, backlog);
        } else {
            this->emit(
                node::net::Server::EVENT_ERROR,
                Error::create(Error::ILLEGAL_ARGUMENT));
            return false;
        }
    }

    virtual Boolean listen(
        String::CPtr path,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (handle_) return false;

        if (callback) this->once(node::net::Server::EVENT_LISTENING, callback);

        return listen(path, -1, -1);
    }

    virtual Boolean close(
        JsFunction::Ptr callback = JsFunction::null()) {
        if (!handle_) return false;

        if (callback) this->once(node::net::Server::EVENT_CLOSE, callback);

        handle_->close();
        handle_ = NULL;
        emitCloseIfDrained();
        return true;
    }

 public:
    void ref() {
        if (handle_) handle_->ref();
    }

    void unref() {
        if (handle_) handle_->unref();
    }

 private:
    static uv::Stream* createServerHandle(
        String::CPtr address,
        Int port = -1,
        Int addressType = -1,
        int fd = -1) {
        uv::Stream* handle;
        if (fd >= 0) {
            uv_handle_type type = uv::Handle::guessHandleType(fd);
            if (type == UV_NAMED_PIPE) {
                uv::Pipe* pipe = new uv::Pipe();
                pipe->open(fd);
                // pipe->readable = true;
                // pipe->writable = true;
                return pipe;
            } else {
                return NULL;
            }
        } else if (port == -1 && addressType == -1) {
            handle = new uv::Pipe();
        } else {
            handle = new uv::Tcp();
        }

        Int r = 0;
        if (address || port) {
            if (addressType == 6) {
                uv::Tcp* tcp = static_cast<uv::Tcp*>(handle);
                r = tcp->bind6(address, port);
            } else if (addressType == 4) {
                uv::Tcp* tcp = static_cast<uv::Tcp*>(handle);
                r = tcp->bind(address, port);
            } else {
                uv::Pipe* pipe = static_cast<uv::Pipe*>(handle);
                r = pipe->bind(address);
            }
        }

        if (r) {
            handle->close();
            return NULL;
        } else {
            return handle;
        }
    }

    Boolean listen(
        String::CPtr address,
        Int port,
        Int addressType,
        Int backlog = 0,
        int fd = -1) {
        if (!handle_) {
            handle_ = createServerHandle(address, port, addressType, fd);
            if (!handle_) {
                typename EmitError::Ptr emitError(new EmitError(this));
                process::nextTick(emitError);
                return false;
            }
        }

        typename OnConnection::Ptr onConnection(new OnConnection(this));
        handle_->setOnConnection(onConnection);
        handle_->setOwner(this);

        Int r = handle_->listen(backlog ? backlog : 511);
        if (r) {
            handle_->close();
            handle_ = NULL;
            typename EmitError::Ptr emitError(new EmitError(this));
            process::nextTick(emitError);
            return false;
        } else {
            StringBuffer::Ptr key = StringBuffer::create();
            key->append(String::valueOf(addressType));
            key->appendChar(':');
            key->append(address);
            key->appendChar(':');
            key->append(String::valueOf(port));
            connectionKey_ = key->toString();
            typename EmitListening::Ptr emitListening(new EmitListening(this));
            process::nextTick(emitListening);
            return true;
        }
    }

    void emitCloseIfDrained() {
        if (handle_ || connections_) return;

        typename EmitClose::Ptr emitClose(new EmitClose(this));
        process::nextTick(emitClose);
    }

 private:
    class OnConnection : LIBJ_JS_FUNCTION_TEMPLATE(OnConnection)
     public:
        OnConnection(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            Value client = args->get(0);
            uv::Stream* clientHandle = NULL;
            uv::Pipe* pipe;
            uv::Tcp* tcp;
            if (to<uv::Pipe*>(client, &pipe)) {
                clientHandle = pipe;
            } else if (to<uv::Tcp*>(client, &tcp)) {
                clientHandle = tcp;
            }
            if (!clientHandle) {
                self_->emit(
                    node::net::Server::EVENT_ERROR,
                    node::uv::Error::last());
                return Error::ILLEGAL_STATE;
            }

            if (self_->maxConnections_ &&
                self_->connections_ >= self_->maxConnections_) {
                clientHandle->close();
                return Status::OK;
            }

            Socket::Ptr socket = Socket::create(
                clientHandle,
                self_->hasFlag(ALLOW_HALF_OPEN));
            socket->setFlag(Socket::READABLE);
            socket->setFlag(Socket::WRITABLE);

            clientHandle->readStart();

            self_->connections_++;
            JsFunction::Ptr removeConnection(new RemoveConnection(self_));
            socket->on(node::net::Server::EVENT_CLOSE, removeConnection);

            self_->emit(node::net::Server::EVENT_CONNECTION, socket);
            socket->emit(Socket::EVENT_CONNECT);
            return Status::OK;
        }

     private:
        Server* self_;
    };

    class RemoveConnection : LIBJ_JS_FUNCTION_TEMPLATE(RemoveConnection)
     public:
        RemoveConnection(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->connections_--;
            self_->emitCloseIfDrained();
            return Status::OK;
        }

     private:
        Server* self_;
    };

    class EmitClose : LIBJ_JS_FUNCTION_TEMPLATE(EmitClose)
     public:
        EmitClose(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_DESTROY, "destroy");

            self_->emit(node::net::Server::EVENT_CLOSE);
            self_->emit(EVENT_DESTROY);
            return Status::OK;
        }

     private:
        Server* self_;
    };

    class EmitError : LIBJ_JS_FUNCTION_TEMPLATE(EmitError)
     public:
        EmitError(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(
                node::net::Server::EVENT_ERROR,
                node::uv::Error::last());
            return Status::OK;
        }

     private:
        Server* self_;
    };

    class EmitListening : LIBJ_JS_FUNCTION_TEMPLATE(EmitListening)
     public:
        EmitListening(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(node::net::Server::EVENT_LISTENING);
            return Status::OK;
        }

     private:
        Server* self_;
    };

 protected:
    class OnDestroy : LIBJ_JS_FUNCTION_TEMPLATE(OnDestroy)
     public:
        OnDestroy(typename Server::Ptr srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            assert(!self_->handle_);
            assert(!self_->connections_);
            self_->removeAllListeners();
            return Status::OK;
        }

     private:
        typename Server::Ptr self_;
    };

 public:
    enum Flag {
        ALLOW_HALF_OPEN      = 1 << 0,
        HTTP_ALLOW_HALF_OPEN = 1 << 1,
    };

 private:
    uv::Stream* handle_;
    Size connections_;
    Size maxConnections_;
    String::CPtr pipeName_;
    String::CPtr connectionKey_;

 protected:
    Server()
        : handle_(NULL)
        , connections_(0)
        , maxConnections_(0)
        , pipeName_(String::null())
        , connectionKey_(String::null()) {}
};

}  // namespace net
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_NET_SERVER_H_
