// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_GTEST_GTEST_NET_COMMON_H_
#define LIBNODE_GTEST_GTEST_NET_COMMON_H_

#include <libnode/net.h>

#include "./gtest_common.h"

namespace libj {
namespace node {

extern const Size NUM_CONNS;

class GTestNetServerOnData : LIBJ_JS_FUNCTION(GTestNetServerOnData)
 public:
    GTestNetServerOnData(net::Socket::Ptr sock) : sock_(sock) {}

    virtual Value operator()(JsArray::Ptr args) {
        sock_->write(args->get(0));
        return Status::OK;
    }

 private:
    net::Socket::Ptr sock_;
};

class GTestNetServerOnEnd : LIBJ_JS_FUNCTION(GTestNetServerOnEnd)
 public:
    GTestNetServerOnEnd(
        net::Server::Ptr srv,
        net::Socket::Ptr sock)
        : srv_(srv)
        , sock_(sock) {}

    virtual Value operator()(JsArray::Ptr args) {
        static UInt count = 0;

        sock_->end();

        count++;
        if (count >= NUM_CONNS) srv_->close();
        return Status::OK;
    }

 private:
    net::Server::Ptr srv_;
    net::Socket::Ptr sock_;
};

class GTestNetServerOnConnection : LIBJ_JS_FUNCTION(GTestNetServerOnConnection)
 public:
    GTestNetServerOnConnection(net::Server::Ptr srv) : srv_(srv) {}

    virtual Value operator()(JsArray::Ptr args) {
        net::Socket::Ptr sock = args->getPtr<net::Socket>(0);
        GTestNetServerOnData::Ptr onData(
            new GTestNetServerOnData(sock));
        GTestNetServerOnEnd::Ptr onEnd(
            new GTestNetServerOnEnd(srv_, sock));
        sock->on(net::Socket::EVENT_DATA, onData);
        sock->on(net::Socket::EVENT_END, onEnd);
        return Status::OK;
    }

 private:
    net::Server::Ptr srv_;
};

class GTestNetSocketOnConnect : LIBJ_JS_FUNCTION(GTestNetSocketOnConnect)
 public:
    GTestNetSocketOnConnect(net::Socket::Ptr sock) : sock_(sock) {}

    virtual Value operator()(JsArray::Ptr args) {
        sock_->write(String::create("a"));
        sock_->write(String::create("b"));
        sock_->end(String::create("c"));
        return Status::OK;
    }

 private:
    net::Socket::Ptr sock_;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_GTEST_GTEST_NET_COMMON_H_

