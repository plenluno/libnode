// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_NET_SOCKET_IMPL_H_
#define LIBNODE_SRC_NET_SOCKET_IMPL_H_

#include <assert.h>

#include "libnode/net.h"
#include "libnode/process.h"
#include "libnode/string_decoder.h"
#include "libnode/timer.h"
#include "libnode/uv/error.h"

#include "../flag.h"
#include "../uv/pipe.h"
#include "../uv/tcp.h"

namespace libj {
namespace node {
namespace http {

class Parser;
class OutgoingMessage;

}  // namespace http
}  // namespace node
}  // namespace libj

namespace libj {
namespace node {
namespace net {

class SocketImpl
    : public FlagMixin
    , LIBNODE_NET_SOCKET(SocketImpl)
 public:
    static Ptr create(JsObject::CPtr options = JsObject::null()) {
        LIBJ_STATIC_SYMBOL_DEF(symFd,            "fd");
        LIBJ_STATIC_SYMBOL_DEF(symHandle,        "handle");
        LIBJ_STATIC_SYMBOL_DEF(symAllowHalfOpen, "allowHalfOpen");

        if (options) {
            Boolean allowHalfOpen = false;
            to<Boolean>(options->get(symAllowHalfOpen), &allowHalfOpen);

            int fd;
            if (to<int>(options->get(symFd), &fd)) {
                return create(fd, allowHalfOpen);
            } else {
                uv::Stream* handle = NULL;
                to<uv::Stream*>(options->get(symHandle), &handle);
                return create(handle, allowHalfOpen);
            }
        } else {
            return create(static_cast<uv::Stream*>(NULL), false);
        }
    }

    static Ptr create(int fd, Boolean allowHalfOpen) {
        SocketImpl* sock = new SocketImpl();
        uv::Pipe* pipe = new uv::Pipe();
        pipe->open(fd);
        OnRead::Ptr onRead(new OnRead(sock, pipe));
        pipe->setOnRead(onRead);

        sock->handle_ = pipe;
        sock->setFlag(READABLE);
        sock->setFlag(WRITABLE);
        if (allowHalfOpen) sock->setFlag(ALLOW_HALF_OPEN);
        return Ptr(sock);
    }

    static Ptr create(uv::Stream* handle, Boolean allowHalfOpen) {
        SocketImpl* sock = new SocketImpl();
        if (handle) {
            OnRead::Ptr onRead(new OnRead(sock, handle));
            handle->setOnRead(onRead);
            sock->handle_ = handle;
        }
        if (allowHalfOpen) sock->setFlag(ALLOW_HALF_OPEN);
        return Ptr(sock);
    }

    void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (timeout) {
            startTimer(timeout);
            if (callback) {
                once(EVENT_TIMEOUT, callback);
            }
        } else {
            finishTimer();
            if (callback) {
                removeListener(EVENT_TIMEOUT, callback);
            }
        }
    }

    Boolean setNoDelay(Boolean noDelay) {
        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return !tcp->setNoDelay(noDelay);
        } else {
            return false;
        }
    }

    Boolean setKeepAlive(
        Boolean enable = false,
        UInt initialDelay = 0) {
        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return !tcp->setKeepAlive(enable, initialDelay / 1000);
        } else {
            return false;
        }
    }

    JsObject::Ptr address() {
        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return tcp->getSockName();
        } else {
            return JsObject::null();
        }
    }

    String::CPtr remoteAddress() {
        LIBJ_STATIC_SYMBOL_DEF(symAddress, "address");

        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            return tcp->getPeerName()->getCPtr<String>(symAddress);
        } else {
            return String::null();
        }
    }

    Int remotePort() {
        LIBJ_STATIC_SYMBOL_DEF(symPort, "port");

        if (handle_ && handle_->type() == UV_TCP) {
            uv::Tcp* tcp = static_cast<uv::Tcp*>(handle_);
            Int port = -1;
            to<Int>(tcp->getPeerName()->get(symPort), &port);
            return port;
        } else {
            return -1;
        }
    }

    Boolean pause() {
        setFlag(PAUSED);
        return handle_ && !hasFlag(CONNECTING) && !handle_->readStop();
    }

    Boolean resume() {
        unsetFlag(PAUSED);
        return handle_ && !hasFlag(CONNECTING) && !handle_->readStart();
    }

    void ref() {
        if (handle_) handle_->ref();
    }

    void unref() {
        if (handle_) handle_->unref();
    }

    void active() {
        if (hasTimer()) {
            finishTimer();
            startTimer(timeout_);
        }
    }

    Boolean write(
        const Value& data,
        Buffer::Encoding enc = Buffer::NONE) {
        return write(data, enc, JsFunction::null());
    }

    Boolean write(
        const Value& data,
        JsFunction::Ptr cb) {
        return write(data, Buffer::NONE, cb);
    }

    Boolean write(
        const Value& data,
        Buffer::Encoding enc,
        JsFunction::Ptr cb) {
        Buffer::CPtr buf;
        String::CPtr str = toCPtr<String>(data);
        if (str) {
            if (enc == Buffer::NONE) enc = Buffer::UTF8;
            buf = Buffer::create(str, enc);
        } else {
            buf = toCPtr<Buffer>(data);
        }
        if (!buf) return false;

        if (hasFlag(CONNECTING)) {
            connectQueueSize_ += buf->length();
            if (!connectBufQueue_) {
                assert(!connectCbQueue_);
                connectBufQueue_ = JsArray::create();
                connectCbQueue_ = JsArray::create();
            }
            connectBufQueue_->push(buf);
            connectCbQueue_->push(cb);
            return false;
        }

        return writeBuffer(buf, cb);
    }

    Boolean end(
        const Value& data = UNDEFINED,
        Buffer::Encoding enc = Buffer::NONE) {
        if (hasFlag(CONNECTING) && !hasFlag(SHUTDOWN_QUEUED)) {
            if (!data.isUndefined()) write(data, enc);
            unsetFlag(WRITABLE);
            setFlag(SHUTDOWN_QUEUED);
        }

        if (!hasFlag(WRITABLE)) {
            return false;
        } else {
            unsetFlag(WRITABLE);
        }

        if (!data.isUndefined()) write(data, enc);

        if (!hasFlag(READABLE)) {
            return destroySoon();
        } else {
            uv::Shutdown* req = handle_->shutdown();
            if (req) {
                AfterShutdown::Ptr afterShutdown(new AfterShutdown(this));
                req->onComplete = afterShutdown;
                return true;
            } else {
                destroy(uv::Error::last());
                return false;
            }
        }
    }

    Boolean destroy() {
        return destroy(libj::Error::null());
    }

    Boolean destroy(libj::Error::CPtr err) {
        return destroy(err, JsFunction::null());
    }

    Boolean destroySoon() {
        if (hasFlag(DESTROYED)) return false;

        unsetFlag(WRITABLE);
        setFlag(DESTROY_SOON);
        return destroy();
    }

    Boolean readable() const {
        return hasFlag(READABLE);
    }

    Boolean writable() const {
        return hasFlag(WRITABLE);
    }

    Buffer::Encoding encoding() const {
        return decoder_ ? decoder_->encoding() : Buffer::NONE;
    }

    Boolean setEncoding(Buffer::Encoding enc) {
        switch (enc) {
        case Buffer::UTF8:
        case Buffer::UTF16BE:
        case Buffer::UTF16LE:
        case Buffer::UTF32BE:
        case Buffer::UTF32LE:
        case Buffer::HEX:
        case Buffer::NONE:
            decoder_ = StringDecoder::create(enc);
            return true;
        default:
            decoder_ = StringDecoder::null();
            return false;
        }
    }

    void setOnData(JsFunction::Ptr onData) {
        if (onData_)
            removeListener(EVENT_DATA, onData_);
        if (onData)
            addListener(EVENT_DATA, onData);
        onData_ = onData;
    }

    void setOnEnd(JsFunction::Ptr onEnd) {
        if (onEnd_)
            removeListener(EVENT_END, onEnd_);
        if (onEnd)
            addListener(EVENT_END, onEnd);
        onEnd_ = onEnd;
    }

    http::Parser* parser() const {
        return parser_;
    }

    void setParser(http::Parser* parser) {
        parser_ = parser;
    }

    http::OutgoingMessage* httpMessage() const {
        return httpMessage_;
    }

    void setHttpMessage(http::OutgoingMessage* msg) {
        httpMessage_ = msg;
    }

    Boolean connect(
        String::CPtr path,
        JsFunction::Ptr cb = JsFunction::null()) {
        if (path) {
            connect(0, String::null(), String::null(), path, cb);
            return true;
        } else {
            return false;
        }
    }

    Boolean connect(
        Int port,
        String::CPtr host = String::null(),
        JsFunction::Ptr cb = JsFunction::null()) {
        if (port < 0) {
            return false;
        } else {
            connect(port, host, String::null(), String::null(), cb);
            return true;
        }
    }

    Boolean connect(
        JsObject::CPtr options,
        JsFunction::Ptr cb = JsFunction::null()) {
        LIBJ_STATIC_SYMBOL_DEF(symPath,         "path");
        LIBJ_STATIC_SYMBOL_DEF(symPort,         "port");
        LIBJ_STATIC_SYMBOL_DEF(symHost,         "host");
        LIBJ_STATIC_SYMBOL_DEF(symLocalAddress, "localAddress");

        if (!options) return false;

        Int port = -1;
        to<Int>(options->get(symPort), &port);
        String::CPtr path = options->getCPtr<String>(symPath);
        String::CPtr host = options->getCPtr<String>(symHost);
        String::CPtr localAddress = options->getCPtr<String>(symLocalAddress);

        if (path) {
            return connect(path, cb);
        } else if (port >= 0) {
            connect(port, host, localAddress, String::null(), cb);
            return true;
        } else {
            return false;
        }
    }

 private:
    static void initSocketHandle(SocketImpl* self) {
        self->flags_ = 0;
        self->pendingWriteReqs_ = 0;
        self->connectQueueSize_ = 0;
        self->bytesRead_ = 0;
        self->bytesDispatched_ = 0;

        uv::Stream* handle  = self->handle_;
        if (handle) {
            OnRead::Ptr onRead(new OnRead(self, handle));
            handle->setOnRead(onRead);
        }
    }

    static void connect(
        SocketImpl* self,
        uv::Pipe* handle,
        String::CPtr path) {
        assert(self->hasFlag(CONNECTING));

        uv::Connect* creq = handle->connect(path);

        if (creq) {
            AfterConnect::Ptr afterConnect(new AfterConnect(self, creq));
            creq->onComplete = afterConnect;
        } else {
            self->destroy(uv::Error::last());
        }
    }

    static void connect(
        SocketImpl* self,
        uv::Tcp* handle,
        String::CPtr address,
        Int port,
        Int addressType,
        String::CPtr localAddress) {
        assert(self->hasFlag(CONNECTING));

        if (localAddress) {
            Int r;
            if (addressType == 6) {
                r = handle->bind6(localAddress);
            } else {
                r = handle->bind(localAddress);
            }

            if (r) {
                self->destroy(uv::Error::last());
                return;
            }
        }

        uv::Connect* creq;
        if (addressType == 6) {
            creq = handle->connect6(address, port);
        } else {  // addressType == 4
            creq = handle->connect(address, port);
        }

        if (creq) {
            AfterConnect::Ptr afterConnect(new AfterConnect(self, creq));
            creq->onComplete = afterConnect;
        } else {
            self->destroy(uv::Error::last());
        }
    }

    void connect(
        Int port,
        String::CPtr host,
        String::CPtr localAddress,
        String::CPtr path,
        JsFunction::Ptr cb) {
        LIBJ_STATIC_SYMBOL_DEF(symLocalhost4, "127.0.0.1");

        Boolean pipe = !!path;

        if (hasFlag(DESTROYED) || !handle_) {
            handle_ = pipe
                ? reinterpret_cast<uv::Stream*>(new uv::Pipe())
                : reinterpret_cast<uv::Stream*>(new uv::Tcp());
            initSocketHandle(this);
        }

        if (cb) on(EVENT_CONNECT, cb);

        active();
        setFlag(CONNECTING);
        setFlag(WRITABLE);

        if (pipe) {
            uv::Pipe* pipe = reinterpret_cast<uv::Pipe*>(handle_);
            connect(this, pipe, path);
        } else {
            uv::Tcp* tcp = reinterpret_cast<uv::Tcp*>(handle_);
            if (!host) {
                connect(this, tcp, symLocalhost4, port, 4, String::null());
            } else {
                // TODO(plenluno): dns lookup

                Int addressType = net::isIP(host);
                if (addressType) {
                    connect(this, tcp, host, port, addressType, localAddress);
                } else {
                    libj::Error::CPtr err =
                        libj::Error::create(libj::Error::ILLEGAL_ARGUMENT);
                    EmitError2::Ptr emitErr(new EmitError2(this, err));
                    process::nextTick(emitErr);
                }
            }
        }
    }

    void connectQueueCleanUp() {
        unsetFlag(CONNECTING);
        connectQueueSize_ = 0;
        connectBufQueue_ = JsArray::null();
        connectCbQueue_ = JsArray::null();
    }

    Boolean destroy(libj::Error::CPtr err, JsFunction::Ptr cb) {
        #define FIRE_ERROR_CALLBACKS \
            if (cb) cb->call(err); \
            if (err && !hasFlag(ERROR_EMITTED)) { \
                EmitError::Ptr emitErr(new EmitError(this, err)); \
                process::nextTick(emitErr); \
                setFlag(ERROR_EMITTED); \
            }

        if (hasFlag(DESTROYED)) {
            FIRE_ERROR_CALLBACKS;
            return false;
        }

        connectQueueCleanUp();
        unsetFlag(READABLE);
        unsetFlag(WRITABLE);
        finishTimer();

        if (handle_) {
            handle_->close();
            handle_->setOnRead(JsFunction::null());
            handle_ = NULL;
        }

        FIRE_ERROR_CALLBACKS;
        EmitClose::Ptr emitClose(new EmitClose(this, err));
        process::nextTick(emitClose);
        setFlag(DESTROYED);

        #if 0
        if (this.server) {
            this.server._connections--;
            if (this.server._emitCloseIfDrained) {
                this.server._emitCloseIfDrained();
            }
        }
        #endif

        return true;

        #undef FIRE_ERROR_CALLBACKS
    }

    Boolean writeBuffer(Buffer::CPtr buf, JsFunction::Ptr cb) {
        active();

        if (!handle_) {
            destroy(libj::Error::create(Error::ILLEGAL_STATE), cb);
            return false;
        }

        uv::Write* req = handle_->writeBuffer(buf);

        if (!req) {
            destroy(uv::Error::last(), cb);
            return false;
        }

        AfterWrite::Ptr afterWrite(new AfterWrite(this, req));
        req->onComplete = afterWrite;
        req->cb = cb;

        pendingWriteReqs_++;
        bytesDispatched_ += req->bytes;
        return true;
    }

 private:
    class OnRead : LIBJ_JS_FUNCTION(OnRead)
     private:
        SocketImpl* self_;
        uv::Stream* handle_;

     public:
        OnRead(
            SocketImpl* sock,
            uv::Stream* handle)
            : self_(sock)
            , handle_(handle) {}

        Value operator()(JsArray::Ptr args) {
            assert(self_->handle_ == handle_);

            self_->active();

            Buffer::CPtr buffer = args->getCPtr<Buffer>(0);
            if (buffer) {
                if (self_->decoder_) {
                    String::CPtr str = self_->decoder_->write(buffer);
                    if (str && str->length()) {
                        self_->emit(EVENT_DATA, str);
                    }
                } else {
                    self_->emit(EVENT_DATA, buffer);
                }

                self_->bytesRead_ += buffer->length();
                // if (self_->onData_) self_->onData_->call(buffer);
            } else {
                uv::Error::CPtr err = uv::Error::last();
                if (err->code() == uv::Error::_EOF) {
                    self_->unsetFlag(READABLE);
                    assert(!self_->hasFlag(GOT_EOF));
                    self_->setFlag(GOT_EOF);

                    if (!self_->hasFlag(WRITABLE)) self_->destroy();
                    if (!self_->hasFlag(ALLOW_HALF_OPEN)) self_->end();

                    #if 0
                    if (self_->decoder_) {
                        ret = self_->decoder_->end();
                        if (ret) self_->emit(EVENT_DATA, ret);
                    }
                    #endif

                    self_->emit(EVENT_END);
                    // if (self_->onEnd_) (*self_->onEnd_)();
                } else if (err->code() == uv::Error::_ECONNRESET) {
                    self_->destroy();
                } else {
                    self_->destroy(err);
                }
            }
            return Status::OK;
        }
    };

    class AfterShutdown : LIBJ_JS_FUNCTION(AfterShutdown)
     private:
        SocketImpl* self_;

     public:
        AfterShutdown(SocketImpl* self) : self_(self) {}

        Value operator()(JsArray::Ptr args) {
            if (self_->hasFlag(DESTROYED)) {
                return Status::OK;
            }

            if (!self_->hasFlag(DESTROYED) &&
                (self_->hasFlag(GOT_EOF) || !self_->hasFlag(READABLE))) {
                self_->destroy();
            }
            return Status::OK;
        }
    };

    class AfterWrite : LIBJ_JS_FUNCTION(AfterWrite)
     private:
        SocketImpl* self_;
        uv::Write* req_;

     public:
        AfterWrite(
            SocketImpl* sock,
            uv::Write* req)
            : self_(sock)
            , req_(req) {}

        Value operator()(JsArray::Ptr args) {
            if (self_->hasFlag(DESTROYED)) {
                return libj::Error::ILLEGAL_STATE;
            }

            int status;
            to<int>(args->get(0), &status);
            if (status) {
                uv::Error::CPtr err = uv::Error::last();
                self_->destroy(err, req_->cb);
                return err->code();
            }

            self_->active();
            self_->pendingWriteReqs_--;
            if (self_->pendingWriteReqs_ == 0) {
                self_->emit(EVENT_DRAIN);
            }

            if (req_->cb) (*req_->cb)();

            if (self_->pendingWriteReqs_ == 0 &&
                self_->hasFlag(DESTROY_SOON)) {
                self_->destroy();
            }
            return Status::OK;
        }
    };

    class AfterConnect : LIBJ_JS_FUNCTION(AfterConnect)
     private:
        SocketImpl* self_;
        uv::Connect* req_;

     public:
        AfterConnect(
            SocketImpl* sock,
            uv::Connect* req)
            : self_(sock)
            , req_(req) {}

        Value operator()(JsArray::Ptr args) {
            if (self_->hasFlag(DESTROYED)) return Status::OK;

            assert(self_->hasFlag(CONNECTING));
            self_->setFlag(CONNECTING);

            int status;
            Boolean readable;
            Boolean writable;
            to<int>(args->get(0), &status);
            to<Boolean>(args->get(3), &readable);
            to<Boolean>(args->get(4), &writable);
            if (!status) {
                if (readable) {
                    self_->setFlag(READABLE);
                } else {
                    self_->unsetFlag(READABLE);
                }
                if (writable) {
                    self_->setFlag(WRITABLE);
                } else {
                    self_->unsetFlag(WRITABLE);
                }

                self_->active();

                if (readable && !self_->hasFlag(PAUSED)) {
                    self_->handle_->readStart();
                }

                if (self_->connectQueueSize_) {
                    JsArray::Ptr connectBufQueue = self_->connectBufQueue_;
                    JsArray::Ptr connectCbQueue = self_->connectCbQueue_;
                    Size len = connectBufQueue->length();
                    assert(connectCbQueue->length() == len);
                    for (Size i = 0; i < len; i++) {
                        JsFunction::Ptr cb =
                            connectCbQueue->getPtr<JsFunction>(i);
                        self_->write(connectBufQueue->get(i), cb);
                    }
                    self_->connectQueueCleanUp();
                }

                self_->emit(EVENT_CONNECT);

                if (self_->hasFlag(SHUTDOWN_QUEUED)) {
                    self_->unsetFlag(SHUTDOWN_QUEUED);
                    self_->end();
                }
            } else {
                self_->connectQueueCleanUp();
                self_->destroy(uv::Error::last());
            }
            return Status::OK;
        }
    };

    class EmitError : LIBJ_JS_FUNCTION(EmitError)
     private:
        SocketImpl* self_;
        libj::Error::CPtr error_;

     public:
        EmitError(
            SocketImpl* sock,
            libj::Error::CPtr err)
            : self_(sock)
            , error_(err) {}

        Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_ERROR);
            return Status::OK;
        }
    };

    class EmitError2 : LIBJ_JS_FUNCTION(EmitError2)
     private:
        SocketImpl* self_;
        libj::Error::CPtr error_;

     public:
        EmitError2(
            SocketImpl* sock,
            libj::Error::CPtr err)
            : self_(sock)
            , error_(err) {}

        Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_ERROR, error_);
            self_->destroy();
            return Status::OK;
        }
    };

    class EmitClose : LIBJ_JS_FUNCTION(EmitClose)
     private:
        SocketImpl* self_;
        libj::Error::CPtr error_;

     public:
        EmitClose(
            SocketImpl* sock,
            libj::Error::CPtr err)
            : self_(sock)
            , error_(err) {}

        Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_CLOSE, !!error_);
            return Status::OK;
        }
    };

    class OnTimeout : LIBJ_JS_FUNCTION(OnTimeout)
     private:
        SocketImpl* self_;

     public:
        OnTimeout(SocketImpl* self) : self_(self) {}

        Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_TIMEOUT);
            return Status::OK;
        }
    };

    Boolean hasTimer() {
        return !timer_.isUndefined();
    }

    void startTimer(Int timeout) {
        if (hasTimer()) finishTimer();

        JsFunction::Ptr onTimeout(new OnTimeout(this));
        timer_ = node::setTimeout(onTimeout, timeout);
        timeout_ = timeout;
    }

    void finishTimer() {
        clearTimeout(timer_);
        timer_ = UNDEFINED;
        timeout_ = 0;
    }

 public:
    enum Flag {
        READABLE        = 1 << 0,
        WRITABLE        = 1 << 1,
        CONNECTING      = 1 << 2,
        PAUSED          = 1 << 3,
        GOT_EOF         = 1 << 4,
        DESTROYED       = 1 << 5,
        DESTROY_SOON    = 1 << 6,
        SHUTDOWN        = 1 << 7,
        SHUTDOWN_QUEUED = 1 << 8,
        ERROR_EMITTED   = 1 << 9,
        ALLOW_HALF_OPEN = 1 << 10,
    };

 private:
    uv::Stream* handle_;
    Value timer_;
    Int timeout_;
    Size pendingWriteReqs_;
    Size connectQueueSize_;
    JsArray::Ptr connectBufQueue_;
    JsArray::Ptr connectCbQueue_;
    Size bytesRead_;
    Size bytesDispatched_;
    StringDecoder::Ptr decoder_;
    JsFunction::Ptr onData_;
    JsFunction::Ptr onEnd_;
    http::Parser* parser_;
    http::OutgoingMessage* httpMessage_;
    events::EventEmitter::Ptr ee_;

    SocketImpl()
        : handle_(NULL)
        , timer_(UNDEFINED)
        , timeout_(0)
        , pendingWriteReqs_(0)
        , connectQueueSize_(0)
        , connectBufQueue_(JsArray::null())
        , connectCbQueue_(JsArray::null())
        , bytesRead_(0)
        , bytesDispatched_(0)
        , decoder_(StringDecoder::null())
        , onData_(JsFunction::null())
        , onEnd_(JsFunction::null())
        , parser_(NULL)
        , httpMessage_(NULL)
        , ee_(events::EventEmitter::create()) {}

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_NET_SOCKET_IMPL_H_
