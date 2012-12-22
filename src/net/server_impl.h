// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_NET_SERVER_IMPL_H_
#define LIBNODE_SRC_NET_SERVER_IMPL_H_

#include <libj/string_buffer.h>
#include <libnode/net.h>

#include "./socket_impl.h"
#include "../flag.h"

namespace libj {
namespace node {
namespace net {

class ServerImpl
    : public FlagMixin
    , LIBNODE_NET_SERVER(ServerImpl)
 public:
    static Symbol::CPtr EVENT_DESTROY;
    static Symbol::CPtr OPTION_ALLOW_HALF_OPEN;

    static Ptr create(
        JsObject::CPtr options = JsObject::null(),
        JsFunction::Ptr listener = JsFunction::null()) {
        Ptr server(new ServerImpl());

        if (options) {
            Boolean allowHalfOpen = false;
            to<Boolean>(options->get(OPTION_ALLOW_HALF_OPEN), &allowHalfOpen);
            if (allowHalfOpen) server->setFlag(ALLOW_HALF_OPEN);
        }

        if (listener) {
            server->on(EVENT_CONNECTION, listener);
        }

        JsFunction::Ptr onDestroy(new OnDestroy(server));
        server->on(EVENT_DESTROY, onDestroy);
        return server;
    }

    Value address() {
        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return tcp->getSockName();
        } else if (pipeName_) {
            return pipeName_;
        } else {
            return JsObject::null();
        }
    }

    Size connections() const {
        return connections_;
    }

    Size maxConnections() const {
        return maxConnections_;
    }

    void setMaxConnections(Size max) {
        maxConnections_ = max;
    }

    Boolean listen(
        Int port,
        String::CPtr host = IN_ADDR_ANY,
        Int backlog = 511,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (handle_) return false;

        if (callback) once(EVENT_LISTENING, callback);

        Int addressType = isIP(host);
        if (addressType) {
            return listen(host, port, addressType, backlog);
        } else {
            emit(EVENT_ERROR, Error::create(Error::ILLEGAL_ARGUMENT));
            return false;
        }
    }

    Boolean listen(
        String::CPtr path,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (handle_) return false;

        if (callback) once(EVENT_LISTENING, callback);

        return listen(path, -1, -1);
    }

    Boolean close(
        JsFunction::Ptr callback = JsFunction::null()) {
        if (!handle_) return false;

        if (callback) once(EVENT_CLOSE, callback);

        handle_->close();
        handle_ = NULL;
        emitCloseIfDrained();
        return true;
    }

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
                EmitError::Ptr emitError(new EmitError(this));
                process::nextTick(emitError);
                return false;
            }
        }

        OnConnection::Ptr onConnection(new OnConnection(this));
        handle_->setOnConnection(onConnection);
        handle_->setOwner(this);

        Int r = handle_->listen(backlog ? backlog : 511);
        if (r) {
            handle_->close();
            handle_ = NULL;
            EmitError::Ptr emitError(new EmitError(this));
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
            EmitListening::Ptr emitListening(new EmitListening(this));
            process::nextTick(emitListening);
            return true;
        }
    }

    void emitCloseIfDrained() {
        if (handle_ || connections_) return;

        EmitClose::Ptr emitClose(new EmitClose(this));
        process::nextTick(emitClose);
    }

 private:
    class OnConnection : LIBJ_JS_FUNCTION(OnConnection)
     private:
        ServerImpl* self_;

     public:
        OnConnection(ServerImpl* srv) : self_(srv) {}

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
                self_->emit(EVENT_ERROR, uv::Error::last());
                return Error::ILLEGAL_STATE;
            }

            if (self_->maxConnections_ &&
                self_->connections_ >= self_->maxConnections_) {
                clientHandle->close();
                return Status::OK;
            }

            SocketImpl::Ptr socket = SocketImpl::create(
                clientHandle,
                self_->hasFlag(ALLOW_HALF_OPEN));
            socket->setFlag(SocketImpl::READABLE);
            socket->setFlag(SocketImpl::WRITABLE);

            clientHandle->readStart();

            self_->connections_++;
            JsFunction::Ptr removeConnection(new RemoveConnection(self_));
            socket->on(EVENT_CLOSE, removeConnection);

            self_->emit(EVENT_CONNECTION, socket);
            socket->emit(SocketImpl::EVENT_CONNECT);
            return Status::OK;
        }
    };

    class RemoveConnection : LIBJ_JS_FUNCTION(RemoveConnection)
     private:
        ServerImpl* self_;

     public:
        RemoveConnection(ServerImpl* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->connections_--;
            self_->emitCloseIfDrained();
            return Status::OK;
        }
    };

    class EmitClose : LIBJ_JS_FUNCTION(EmitClose)
     private:
        ServerImpl* self_;

     public:
        EmitClose(ServerImpl* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_CLOSE);
            self_->emit(EVENT_DESTROY);
            return Status::OK;
        }
    };

    class EmitError : LIBJ_JS_FUNCTION(EmitError)
     private:
        ServerImpl* self_;

     public:
        EmitError(ServerImpl* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_ERROR, uv::Error::last());
            return Status::OK;
        }
    };

    class EmitListening : LIBJ_JS_FUNCTION(EmitListening)
     private:
        ServerImpl* self_;

     public:
        EmitListening(ServerImpl* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_LISTENING);
            return Status::OK;
        }
    };

    class OnDestroy : LIBJ_JS_FUNCTION(OnDestroy)
     private:
        ServerImpl::Ptr self_;

     public:
        OnDestroy(ServerImpl::Ptr srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            assert(!self_->handle_);
            assert(!self_->connections_);
            self_->removeAllListeners();
            return Status::OK;
        }
    };

 public:
    enum Flag {
        ALLOW_HALF_OPEN = 1 << 0,
    };

 private:
    uv::Stream* handle_;
    Size connections_;
    Size maxConnections_;
    String::CPtr pipeName_;
    String::CPtr connectionKey_;
    events::EventEmitter::Ptr ee_;

    ServerImpl()
        : handle_(NULL)
        , connections_(0)
        , maxConnections_(0)
        , pipeName_(String::null())
        , connectionKey_(String::null())
        , ee_(events::EventEmitter::create()) {}

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_NET_SERVER_IMPL_H_
