// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/timer.h>

#include <libj/console.h>
#include <libj/status.h>

namespace libj {
namespace node {

class GTestTimerOnTimeout : LIBJ_JS_FUNCTION(GTestTimerOnTimeout)
 public:
    GTestTimerOnTimeout() {}

    void setId(Value id) {
        id_ = id;
    }

    Value operator()(JsArray::Ptr args) {
        clearTimeout(id_);
        clear_++;
        return Status::OK;
    }

    static UInt getClearCount() {
        return clear_;
    }

    static void clearClearCount() {
        clear_ = 0;
    }

 private:
    static UInt clear_;

    Value id_;
};

UInt GTestTimerOnTimeout::clear_ = 0;

TEST(GTestTimer, TestSetTimeout) {
    GTestTimerOnTimeout::Ptr onTimeout1(new GTestTimerOnTimeout());
    Value id1 = setTimeout(onTimeout1, 100);
    onTimeout1->setId(id1);

    GTestTimerOnTimeout::Ptr onTimeout2(new GTestTimerOnTimeout());
    Value id2 = setTimeout(onTimeout2, 75);
    onTimeout2->setId(id2);

    GTestTimerOnTimeout::Ptr onTimeout3(new GTestTimerOnTimeout());
    Value id3 = setTimeout(onTimeout3, 100);
    onTimeout3->setId(id3);

    node::run();
    ASSERT_EQ(3, GTestTimerOnTimeout::getClearCount());
}

TEST(GTestTimer, TestClearTimeout) {
    GTestTimerOnTimeout::clearClearCount();

    GTestTimerOnTimeout::Ptr onTimeout1(new GTestTimerOnTimeout());
    Value id1 = setTimeout(onTimeout1, 1000);
    onTimeout1->setId(id1);

    GTestTimerOnTimeout::Ptr onTimeout2(new GTestTimerOnTimeout());
    Value id2 = setTimeout(onTimeout2, 100);
    onTimeout2->setId(id1);

    node::run();
    ASSERT_EQ(1, GTestTimerOnTimeout::getClearCount());
}

class GTestTimerOnInterval : LIBJ_JS_FUNCTION(GTestTimerOnInterval)
 public:
    GTestTimerOnInterval() : count_(0) {}

    void setId(Value id) {
        id_ = id;
    }

    void setCount(Int cnt) {
        count_ = cnt;
    }

    Value operator()(JsArray::Ptr args) {
        if (count_) {
            console::log(String::valueOf(count_--));
        } else {
            clearInterval(id_);
            clear_++;
        }
        return Status::OK;
    }

    static UInt getClearCount() {
        return clear_;
    }

    static void clearClearCount() {
        clear_ = 0;
    }

 private:
    static UInt clear_;

    Value id_;
    Int count_;
};

UInt GTestTimerOnInterval::clear_ = 0;

TEST(GTestTimer, TestSetInterval) {
    GTestTimerOnInterval::Ptr onInterval1(new GTestTimerOnInterval());
    Value id1 = setInterval(onInterval1, 100);
    onInterval1->setId(id1);
    onInterval1->setCount(6);

    GTestTimerOnInterval::Ptr onInterval2(new GTestTimerOnInterval());
    Value id2 = setInterval(onInterval2, 75);
    onInterval2->setId(id2);
    onInterval2->setCount(8);

    node::run();
    ASSERT_EQ(2, GTestTimerOnInterval::getClearCount());
}

TEST(GTestTimer, TestClearInterval) {
    GTestTimerOnTimeout::clearClearCount();
    GTestTimerOnInterval::clearClearCount();

    GTestTimerOnInterval::Ptr onInterval(new GTestTimerOnInterval());
    Value id1 = setInterval(onInterval, 1000);
    onInterval->setId(id1);
    onInterval->setCount(5);

    GTestTimerOnTimeout::Ptr onTimeout(new GTestTimerOnTimeout());
    Value id2 = setTimeout(onTimeout, 100);
    onTimeout->setId(id1);

    node::run();
    ASSERT_EQ(1, GTestTimerOnTimeout::getClearCount());
    ASSERT_EQ(0, GTestTimerOnInterval::getClearCount());
}

}  // namespace node
}  // namespace libj
