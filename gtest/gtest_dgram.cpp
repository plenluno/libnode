// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/dgram.h>

#include <libj/json.h>
#include <libj/console.h>

#include <assert.h>

namespace libj {
namespace node {

static const UInt NUM_SEND = 7;

class GTestDgramOnMessage : LIBJ_JS_FUNCTION(GTestDgramOnMessage)
 public:
    GTestDgramOnMessage(dgram::Socket::Ptr srv)
        : bufs_(JsArray::create())
        , rinfos_(JsArray::create())
        , server_(&(*srv)) {}

    virtual Value operator()(JsArray::Ptr args) {
        bufs_->add(args->get(0));
        rinfos_->add(args->get(1));
        if (bufs_->length() >= NUM_SEND) {
            server_->close();
        }
        return Status::OK;
    }

    JsArray::Ptr messages() {
        return bufs_;
    }

    JsArray::Ptr rinfos() {
        return rinfos_;
    }

 private:
    JsArray::Ptr bufs_;
    JsArray::Ptr rinfos_;
    dgram::Socket* server_;
};

class GTestDgramAfterSend : LIBJ_JS_FUNCTION(GTestDgramAfterSend)
 public:
    GTestDgramAfterSend(dgram::Socket::Ptr cli)
        : count_(0)
        , client_(&(*cli)) {}

    virtual Value operator()(JsArray::Ptr args) {
        assert(!args->getCPtr<libj::Error>(0));
        assert(libj::to<Size>(args->get(1), static_cast<Size>(0)));
        count_++;
        if (count_ >= NUM_SEND) {
            client_->close();
        }
        return Status::OK;
    }

 private:
    UInt count_;
    dgram::Socket* client_;
};

TEST(GTestDgram, TestDgramSend) {
    Int port = 41234;
    dgram::Socket::Ptr server = dgram::createSocket(dgram::Socket::UDP4);

    GTestDgramOnMessage::Ptr onMessage(new GTestDgramOnMessage(server));
    server->on(dgram::Socket::EVENT_MESSAGE, onMessage);
    server->bind(port);

    String::CPtr hello = String::create("hello");
    String::CPtr localhost = String::create("localhost");
    dgram::Socket::Ptr client = dgram::createSocket(dgram::Socket::UDP4);
    GTestDgramAfterSend::Ptr afterSend(new GTestDgramAfterSend(client));
    for (Size i = 0; i < NUM_SEND; i++) {
        Buffer::Ptr msg = Buffer::create(hello);
        client->send(msg, 0, msg->length(), port, localhost, afterSend);
    }

    node::run();

    String::CPtr address = String::create("address");
    String::CPtr localIP = String::create("127.0.0.1");
    JsArray::CPtr messages = onMessage->messages();
    JsArray::CPtr rinfos = onMessage->rinfos();
    Size numMsgs = messages->length();
    ASSERT_EQ(NUM_SEND, numMsgs);
    for (Size i = 0; i < numMsgs; i++) {
        console::printf(console::LEVEL_INFO, ".");

        String::CPtr msg = messages->getCPtr<Buffer>(i)->toString();
        ASSERT_TRUE(msg->equals(hello));

        JsObject::CPtr rinfo = rinfos->getCPtr<JsObject>(i);
        ASSERT_TRUE(rinfo->getCPtr<String>(address)->equals(localIP));
    }
    console::printf(console::LEVEL_INFO, "\n");
}

}  // namespace node
}  // namespace libj
