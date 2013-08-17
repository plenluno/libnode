// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_DGRAM_SOCKET_H_
#define LIBNODE_DETAIL_DGRAM_SOCKET_H_

#include <libnode/dns.h>
#include <libnode/dgram/socket.h>
#include <libnode/detail/uv/udp.h>
#include <libnode/detail/events/event_emitter.h>

namespace libj {
namespace node {
namespace detail {
namespace dgram {

class Socket : public events::EventEmitter<node::dgram::Socket> {
 public:
    LIBJ_MUTABLE_DEFS(Socket, node::dgram::Socket);

    static Ptr create(Type type, JsFunction::Ptr callback) {
        Socket* sock = new Socket(type);
        sock->on(EVENT_MESSAGE, callback);
        return Ptr(sock);
    }

    virtual void close() {
        if (handle_) {
            stopReceiving();
            handle_->close();
            handle_ = NULL;
            emit(EVENT_CLOSE);
        }
    }

    virtual libj::JsObject::Ptr address() {
        if (handle_) {
            return handle_->getSockName();
        } else {
            return libj::JsObject::null();
        }
    }

    virtual Boolean send(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        JsFunction::Ptr callback = JsFunction::null()) {
        if (!handle_ || !buf) return false;

        Size bufLen = buf->length();
        if (offset >= bufLen || offset + length > bufLen) return false;

        if (bindState_ == UNBOUND) bind(0);

        if (bindState_ != BOUND) {
            if (!sendQueue_) {
                sendQueue_ = JsArray::create();
                on(EVENT_LISTENING, JsFunction::Ptr(new OnListening(this)));
            }
            JsArray::Ptr params = JsArray::create();
            params->add(buf);
            params->add(offset);
            params->add(length);
            params->add(port);
            params->add(address);
            params->add(callback);
            sendQueue_->push(params);
            return true;
        }

        return lookup(
            address,
            JsFunction::Ptr(new AfterLookupInSend(
                this,
                buf,
                offset,
                length,
                port,
                callback)));
    }

    virtual Boolean bind(
        Int port,
        String::CPtr address = String::null(),
        JsFunction::Ptr callback = JsFunction::null()) {
        if (!handle_ || bindState_ != UNBOUND) return false;

        bindState_ = BINDING;

        if (callback) once(EVENT_LISTENING, callback);

        return lookup(
            address,
            JsFunction::Ptr(new AfterLookupInBind(this, port)));
    }

    virtual Boolean setBroadcast(Boolean flag) {
        return handle_ && !handle_->setBroadcast(flag ? 1 : 0);
    }

    virtual Boolean setTTL(Int ttl) {
        return handle_ && !handle_->setTTL(ttl);
    }

    virtual Boolean setMulticastTTL(Int ttl) {
        return handle_ && !handle_->setMulticastTTL(ttl);
    }

    virtual Boolean setMulticastLoopback(Boolean flag) {
        return handle_ && !handle_->setMulticastLoopback(flag);
    }

    virtual Boolean addMembership(
        String::CPtr multicastAddress,
        String::CPtr multicastInterface = String::null()) {
        return handle_ &&
            multicastAddress &&
            !handle_->addMembership(multicastAddress, multicastInterface);
    }

    virtual Boolean dropMembership(
        String::CPtr multicastAddress,
        String::CPtr multicastInterface = String::null()) {
        return handle_ &&
            multicastAddress &&
            !handle_->dropMembership(multicastAddress, multicastInterface);
    }

    virtual void ref() {
        if (handle_) handle_->ref();
    }

    virtual void unref() {
        if (handle_) handle_->unref();
    }

 private:
    Boolean lookup(String::CPtr addr, JsFunction::Ptr callback) {
        LIBJ_STATIC_SYMBOL_DEF(defaultIP4, "0.0.0.0");
        LIBJ_STATIC_SYMBOL_DEF(defaultIP6, "::0");

        if (type_ == UDP4) {
            if (!addr) addr = defaultIP4;
            return dns::lookup(addr, 4, callback);
        } else {
            assert(type_ == UDP6);
            if (!addr) addr = defaultIP6;
            return dns::lookup(addr, 6, callback);
        }
    }

    uv::UdpSend* uvSend(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr ip) {
        if (type_ == UDP4) {
            return handle_->send4(buf, offset, length, port, ip);
        } else {
            assert(type_ == UDP6);
            return handle_->send6(buf, offset, length, port, ip);
        }
    }

    Int uvBind(String::CPtr addr, Int port, Int flags) {
        assert(handle_);
        if (type_ == UDP4) {
            return handle_->bind4(addr, port, flags);
        } else {
            assert(type_ == UDP6);
            return handle_->bind6(addr, port, flags);
        }
    }

    void startListening() {
        assert(handle_);
        handle_->setOnMessage(JsFunction::Ptr(new OnMessage(this)));
        handle_->recvStart();
        receiving_ = true;
        bindState_ = BOUND;
        emit(EVENT_LISTENING);
    }

    void stopReceiving() {
        if (receiving_) {
            handle_->recvStop();
            receiving_ = false;
        }
    }

 private:
    class OnMessage : LIBJ_JS_FUNCTION(OnMessage)
     public:
        OnMessage(Socket* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(EVENT_MESSAGE, args);
            return Status::OK;
        }

     private:
        Socket* self_;
    };

    class OnListening : LIBJ_JS_FUNCTION(OnListening)
        OnListening(Socket* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
            assert(self_->sendQueue_);
            Size len = self_->sendQueue_->length();
            for (Size i = 0; i < len; i++) {
                JsArray::Ptr ps = self_->sendQueue_->getPtr<JsArray>(i);
                Buffer::CPtr buf = ps->getCPtr<Buffer>(0);
                Size offset = to<Size>(ps->get(1));
                Size length = to<Size>(ps->get(2));
                Int port = to<Int>(ps->get(3));
                String::CPtr address = ps->getCPtr<String>(4);
                JsFunction::Ptr callback = ps->getPtr<JsFunction>(5);
                self_->send(buf, offset, length, port, address, callback);
            }
            return Status::OK;
        }

     private:
        Socket* self_;
    };

    class AfterSend : LIBJ_JS_FUNCTION(AfterSend)
     public:
        virtual Value operator()(JsArray::Ptr args) {
            assert(!to<Int>(args->get(0), -1));
            uv::UdpSend* udpSend = to<uv::UdpSend*>(args->get(1));
            if (udpSend && udpSend->cb) {
                udpSend->cb->call(
                    libj::Error::null(),
                    udpSend->buffer->length());
            }
            return Status::OK;
        }
    };

    class AfterLookupInSend : LIBJ_JS_FUNCTION(AfterLookupInSend)
     public:
        AfterLookupInSend(
            Socket* self,
            Buffer::CPtr buf,
            Size offset,
            Size length,
            Int port,
            JsFunction::Ptr callback)
            : self_(self)
            , buf_(buf)
            , offset_(offset)
            , length_(length)
            , port_(port)
            , callback_(callback) {}

        virtual Value operator()(JsArray::Ptr args) {
            libj::Error::CPtr err = args->getCPtr<libj::Error>(0);
            if (err) {
                if (callback_) callback_->call(err);
                self_->emit(EVENT_ERROR, err);
                return err;
            }

            if (!self_->handle_) return Status::OK;

            uv::UdpSend* udpSend = self_->uvSend(
                buf_,
                offset_,
                length_,
                port_,
                args->getCPtr<String>(1));
            if (udpSend) {
                JsFunction::Ptr afterSend(new AfterSend());
                udpSend->onComplete = afterSend;
                udpSend->cb = callback_;
            } else {
                libj::Error::CPtr err = node::uv::Error::last();
                callback_->call(err, 0);
            }
            return Status::OK;
        }

     private:
        Socket* self_;
        Buffer::CPtr buf_;
        Size offset_;
        Size length_;
        Int port_;
        JsFunction::Ptr callback_;
    };

    class AfterLookupInBind : LIBJ_JS_FUNCTION(AfterLookupInBind)
     public:
        AfterLookupInBind(Socket* self, Int port)
            : self_(self)
            , port_(port) {}

        virtual Value operator()(JsArray::Ptr args) {
            libj::Error::CPtr err = args->getCPtr<libj::Error>(0);
            if (err) {
                self_->bindState_ = UNBOUND;
                self_->emit(EVENT_ERROR, err);
                return err;
            }

            if (!self_->handle_) return Status::OK;

            String::CPtr ip = args->getCPtr<String>(1);
            if (self_->uvBind(ip, port_, 0)) {
                err = node::uv::Error::last();
                self_->bindState_ = UNBOUND;
                self_->emit(EVENT_ERROR, err);
                return err;
            }

            self_->startListening();
            return Status::OK;
        }

     private:
        Socket* self_;
        Int port_;
    };

 private:
    enum BindState {
        UNBOUND,
        BINDING,
        BOUND
    };

 private:
    uv::Udp* handle_;
    Type type_;
    Boolean receiving_;
    BindState bindState_;
    JsArray::Ptr sendQueue_;

    Socket(Type type)
        : handle_(new uv::Udp())
        , type_(type)
        , receiving_(false)
        , bindState_(UNBOUND)
        , sendQueue_(JsArray::null()) {}
};

}  // namespace dgram
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_DGRAM_SOCKET_H_
