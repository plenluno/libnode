// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DGRAM_SOCKET_H_
#define LIBNODE_DGRAM_SOCKET_H_

#include <libnode/stream/duplex_stream.h>

namespace libj {
namespace node {
namespace dgram {

class Socket : LIBNODE_EVENT_EMITTER(Socket)
 public:
    enum Type {
        UDP4,
        UDP6
    };

    static Symbol::CPtr EVENT_MESSAGE;
    static Symbol::CPtr EVENT_LISTENING;
    static Symbol::CPtr EVENT_CLOSE;
    static Symbol::CPtr EVENT_ERROR;

    static Ptr create(
        Type type,
        JsFunction::Ptr callback = JsFunction::null());

    virtual void close() = 0;

    virtual JsObject::Ptr address() = 0;

    virtual Boolean send(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean bind(
        Int port,
        JsFunction::Ptr callback) = 0;

    virtual Boolean bind(
        Int port,
        String::CPtr address = String::null(),
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean setBroadcast(Boolean flag) = 0;

    virtual Boolean setTTL(Int ttl) = 0;

    virtual Boolean setMulticastTTL(Int ttl) = 0;

    virtual Boolean setMulticastLoopback(Boolean flag) = 0;

    virtual Boolean addMembership(
        String::CPtr multicastAddress,
        String::CPtr multicastInterface = String::null()) = 0;

    virtual Boolean dropMembership(
        String::CPtr multicastAddress,
        String::CPtr multicastInterface = String::null()) = 0;
};

}  // namespace dgram
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DGRAM_SOCKET_H_
