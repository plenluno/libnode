// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/fs.h>

#include "./gtest_common.h"

namespace libj {
namespace node {

class FsCallback : LIBJ_JS_FUNCTION(FsCallback)
 public:
    FsCallback() : args_(JsArray::null()) {}

    JsArray::CPtr getArgs() { return args_; }

    virtual Value operator()(JsArray::Ptr args) {
        args_ = args;
        return Status::OK;
    }

 private:
    JsArray::Ptr args_;
};

TEST(GTestFs, TestStat) {
    fs::stat(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::stat(String::null(), cb);
    node::run();
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Error>(0));

    fs::stat(String::create("CMakeCache.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<fs::Stats>(1));
}

TEST(GTestFs, TestOpen) {
    fs::open(String::null(), fs::W, JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::open(String::null(), fs::W, cb);
    node::run();
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestClose) {
    fs::close(Object::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::close(Object::null(), cb);
    node::run();
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestRead) {
    fs::read(
        Object::null(),
        Buffer::null(),
        0, 0, 0,
        JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::read(
        Object::null(),
        Buffer::create(),
        1, 1, 1, cb);
    node::run();
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestReadFile) {
    fs::readFile(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::readFile(String::create("CMakeCache.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Buffer>(1));
}

TEST(GTestFs, TestWriteFile) {
    fs::writeFile(
        String::null(), Buffer::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::writeFile(
        String::create("hello.txt"),
        Buffer::create(String::create("hello")),
        cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

}  // namespace node
}  // namespace libj
