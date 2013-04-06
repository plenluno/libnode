// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/async.h>

#include <libj/status.h>
#include <libj/console.h>

namespace libj {
namespace node {

class GTestAsyncTask : LIBJ_JS_FUNCTION(GTestAsyncTask)
 public:
    GTestAsyncTask(UInt i) : i_(i) {}

    virtual Value operator()(JsArray::Ptr args) {
        console::printf(console::LEVEL_INFO, ".");
        return i_;
    }

 private:
    UInt i_;
};

class GTestAsyncCallback : LIBJ_JS_FUNCTION(GTestAsyncCallback)
 public:
    GTestAsyncCallback(
        Async::Ptr async,
        UInt numTasks)
        : sum_(0)
        , count_(0)
        , numTasks_(numTasks)
        , async_(async) {}

    UInt sum() { return sum_; }

    virtual Value operator()(JsArray::Ptr args) {
        UInt res = 0;
        to<UInt>(args->get(0), &res);
        sum_ += res;
        count_++;
        if (count_ >= numTasks_) async_->close();
        return Status::OK;
    }

 private:
    UInt sum_;
    UInt count_;
    UInt numTasks_;
    Async::Ptr async_;
};

static const UInt NUM_TASKS = 9;
static const UInt NUM_THREADS = 5;

TEST(GTestAsync, TestExec) {
    Async::Ptr async = Async::create(NUM_THREADS);

    GTestAsyncCallback::Ptr calllback(
        new GTestAsyncCallback(async, NUM_TASKS));

    UInt sum = 0;
    for (UInt i = 0; i < NUM_TASKS; i++) {
        GTestAsyncTask::Ptr task(new GTestAsyncTask(i));
        async->exec(task, calllback);
        sum += i;
    }
    node::run();
    console::printf(console::LEVEL_INFO, "\n", sum);

    ASSERT_EQ(sum, calllback->sum());
}

}  // namespace node
}  // namespace libj
