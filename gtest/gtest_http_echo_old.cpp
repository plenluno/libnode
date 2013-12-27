// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include "./gtest_http_common.h"
#include "./gtest_net_common.h"

namespace libj {
namespace node {

class GTestHttpEchoOnCloseV09 : LIBJ_JS_FUNCTION(GTestHttpEchoOnCloseV09)
    GTestHttpEchoOnCloseV09(http::Server::Ptr srv) : srv_(srv) {}

    static UInt count() {
        return count_;
    }

    static void clear() {
        count_ = 0;
    }

    virtual Value operator()(JsArray::Ptr args) {
        count_++;
        if (srv_) srv_->close();
        return Status::OK;
    }

 private:
    static UInt count_;

    http::Server::Ptr srv_;
};

UInt GTestHttpEchoOnCloseV09::count_ = 0;

class GTestHttpEchoOnConnectV09 : LIBJ_JS_FUNCTION(GTestHttpEchoOnConnectV09)
 public:
    GTestHttpEchoOnConnectV09(net::Socket::Ptr sock) : sock_(sock) {}

    virtual Value operator()(JsArray::Ptr args) {
        sock_->end(str("GET /index.html\r\n"));
        return Status::OK;
    }

 private:
    net::Socket::Ptr sock_;
};

class GTestHttpEchoOnConnectV10 : LIBJ_JS_FUNCTION(GTestHttpEchoOnConnectV10)
 public:
    GTestHttpEchoOnConnectV10(net::Socket::Ptr sock) : sock_(sock) {}

    virtual Value operator()(JsArray::Ptr args) {
        sock_->end(str(
            "GET /index.html HTTP/1.0\r\n"
            "Content-Length: 3\r\n"
            "\r\n"
            "1.0"));
        return Status::OK;
    }

 private:
    net::Socket::Ptr sock_;
};

TEST(GTestHttpEchoOld, TestHttpV09) {
    const Int port = 10000;

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(
        new GTestHttpServerOnRequest(srv, 1));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(port);

    net::Socket::Ptr socket = net::createConnection(port);
    GTestOnData::Ptr onData(new GTestOnData());
    JsFunction::Ptr onEnd(new GTestOnEnd(onData));
    JsFunction::Ptr onClose(new GTestHttpEchoOnCloseV09(srv));
    JsFunction::Ptr onConnect(new GTestHttpEchoOnConnectV09(socket));
    socket->on(net::Socket::EVENT_DATA, onData);
    socket->on(net::Socket::EVENT_END, onEnd);
    socket->on(net::Socket::EVENT_CLOSE, onClose);
    socket->on(net::Socket::EVENT_CONNECT, onConnect);

    node::run();

    ASSERT_EQ(1, GTestHttpEchoOnCloseV09::count());

    JsArray::CPtr msgs = GTestOnEnd::messages();
    ASSERT_EQ(1, msgs->length());
    ASSERT_TRUE(msgs->getCPtr<String>(0)->isEmpty());

    clearGTestHttpCommon();
    GTestHttpEchoOnCloseV09::clear();
}

TEST(GTestHttpEchoOld, TestHttpV10) {
    const Int port = 10000;

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(
        new GTestHttpServerOnRequest(srv, 1));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(port);

    net::Socket::Ptr socket = net::createConnection(port);
    GTestOnData::Ptr onData(new GTestOnData());
    JsFunction::Ptr onEnd(new GTestOnEnd(onData));
    JsFunction::Ptr onClose(new GTestOnClose());
    JsFunction::Ptr onConnect(new GTestHttpEchoOnConnectV10(socket));
    socket->on(net::Socket::EVENT_DATA, onData);
    socket->on(net::Socket::EVENT_END, onEnd);
    socket->on(net::Socket::EVENT_CLOSE, onClose);
    socket->on(net::Socket::EVENT_CONNECT, onConnect);

    node::run();

    ASSERT_EQ(1, GTestOnClose::count());

    JsArray::CPtr msgs = GTestOnEnd::messages();
    ASSERT_EQ(1, msgs->length());

    Buffer::CPtr res = msgs->getCPtr<Buffer>(0);
    String::CPtr resStr = res->toString();
    String::CPtr status = str("HTTP/1.1 200 OK\r\n");
    String::CPtr header1 = str("Connection: close\r\n");
    String::CPtr header2 = str("Content-Length: 3\r\n");
    String::CPtr header3 = str("Content-Type: text/plain\r\n");
    String::CPtr body = str("\r\n1.0");
    ASSERT_EQ(resStr->length(),
        status->length() +
        header1->length() + header2->length() + header3->length() +
        body->length());
    ASSERT_TRUE(resStr->startsWith(status));
    ASSERT_TRUE(resStr->endsWith(body));
    ASSERT_NE(NO_POS, resStr->indexOf(header1));
    ASSERT_NE(NO_POS, resStr->indexOf(header2));
    ASSERT_NE(NO_POS, resStr->indexOf(header3));

    clearGTestHttpCommon();
}

TEST(GTestHttpEchoOld, TestHttpV10NoContentLength) {
    const Int port = 10000;

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(
        new GTestHttpServerOnRequest(srv, 1, true));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(port);

    net::Socket::Ptr socket = net::createConnection(port);
    GTestOnData::Ptr onData(new GTestOnData());
    JsFunction::Ptr onEnd(new GTestOnEnd(onData));
    JsFunction::Ptr onClose(new GTestOnClose());
    JsFunction::Ptr onConnect(new GTestHttpEchoOnConnectV10(socket));
    socket->on(net::Socket::EVENT_DATA, onData);
    socket->on(net::Socket::EVENT_END, onEnd);
    socket->on(net::Socket::EVENT_CLOSE, onClose);
    socket->on(net::Socket::EVENT_CONNECT, onConnect);

    node::run();

    ASSERT_EQ(1, GTestOnClose::count());

    JsArray::CPtr msgs = GTestOnEnd::messages();
    ASSERT_EQ(1, msgs->length());

    Buffer::CPtr res = msgs->getCPtr<Buffer>(0);
    String::CPtr resStr = res->toString();
    String::CPtr status = str("HTTP/1.1 200 OK\r\n");
    String::CPtr header1 = str("Connection: close\r\n");
    String::CPtr header2 = str("Content-Type: text/plain\r\n");
    String::CPtr body = str("\r\n1.0");
    ASSERT_EQ(resStr->length(),
        status->length() +
        header1->length() + header2->length() +
        body->length());
    ASSERT_TRUE(resStr->startsWith(status));
    ASSERT_TRUE(resStr->endsWith(body));
    ASSERT_NE(NO_POS, resStr->indexOf(header1));
    ASSERT_NE(NO_POS, resStr->indexOf(header2));

    clearGTestHttpCommon();
}

}  // namespace node
}  // namespace libj
