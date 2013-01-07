// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/message_queue.h>

#include <libj/status.h>
#include <libj/thread.h>

namespace libj {
namespace node {

class GTestMsgQueueOnMessage : LIBJ_JS_FUNCTION(GTestMsgQueueOnMessage)
 public:
    GTestMsgQueueOnMessage(
        MessageQueue::Ptr mq,
        UInt numPost)
        : count_(0)
        , numPost_(numPost)
        , msgQueue_(mq) {}

    UInt count() { return count_; }

    virtual Value operator()(JsArray::Ptr args) {
        count_++;
        if (count_ >= numPost_) {
            msgQueue_->stop();
            msgQueue_ = MessageQueue::null();
        }
        return Status::OK;
    }

 private:
    UInt count_;
    UInt numPost_;
    MessageQueue::Ptr msgQueue_;
};

class GTestMsgQueueNodeRun : LIBJ_JS_FUNCTION(GTestMsgQueueNodeRun)
 public:
    virtual Value operator()(JsArray::Ptr args) {
        node::run();
        return Status::OK;
    }
};

class GTestMsgQueuePostMessage : LIBJ_JS_FUNCTION(GTestMsgQueuePostMessage)
 public:
    GTestMsgQueuePostMessage(
        MessageQueue::Ptr mq,
        UInt numPost)
        : numPost_(numPost)
        , msgQueue_(mq) {}

    virtual Value operator()(JsArray::Ptr args) {
        for (UInt i = 0; i < numPost_; i++) {
            msgQueue_->postMessage(i);
        }
        return Status::OK;
    }

 private:
    UInt numPost_;
    MessageQueue::Ptr msgQueue_;
};

static const UInt NUM_POSTS = 7;
static const UInt NUM_THREADS = 5;

TEST(GTestMessageQueue, TestPostMessageSameThread) {
    MessageQueue::Ptr mq = MessageQueue::create();
    mq->start();

    GTestMsgQueueOnMessage::Ptr onMessage(
        new GTestMsgQueueOnMessage(mq, NUM_POSTS));
    mq->on(MessageQueue::EVENT_MESSAGE, onMessage);

    for (UInt i = 0; i < NUM_POSTS; i++) {
        mq->postMessage(String::create("msg"));
    }

    node::run();

    ASSERT_EQ(NUM_POSTS, onMessage->count());
}

TEST(GTestMessageQueue, TestPostMessageMultiThread) {
    MessageQueue::Ptr mq = MessageQueue::create();
    mq->start();

    GTestMsgQueueOnMessage::Ptr onMessage(
        new GTestMsgQueueOnMessage(mq, NUM_POSTS * NUM_THREADS));
    mq->on(MessageQueue::EVENT_MESSAGE, onMessage);

    Thread::Ptr nodeRun(Thread::create(
        Function::Ptr(new GTestMsgQueueNodeRun())));

    JsArray::Ptr threads = JsArray::create();
    for (Size i = 0; i < NUM_THREADS; i++) {
        threads->add(Thread::create(
            Function::Ptr(new GTestMsgQueuePostMessage(mq, NUM_POSTS))));
    }

    nodeRun->start();

    for (Size i = 0; i < NUM_THREADS; i++) {
        threads->getPtr<Thread>(i)->start();
    }

    nodeRun->join();

    for (Size i = 0; i < NUM_THREADS; i++) {
        threads->getPtr<Thread>(i)->join();
    }

    ASSERT_EQ(NUM_POSTS * NUM_THREADS, onMessage->count());
}

}  // namespace node
}  // namespace libj
