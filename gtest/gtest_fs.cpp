// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/fs.h>
#include <libnode/path.h>

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

TEST(GTestFs, TestWriteFile) {
    fs::writeFile(
        String::null(), Buffer::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::writeFile(str("hello.txt"), Buffer::create(str("hello")), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestStat) {
    fs::stat(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::stat(String::null(), cb);
    node::run();
    ASSERT_TRUE(!!cb->getArgs()->getCPtr<Error>(0));

    fs::stat(str("hello.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    fs::Stats::Ptr stats = cb->getArgs()->getPtr<fs::Stats>(1);
    ASSERT_TRUE(!!stats);
    ASSERT_TRUE(stats->isFile());
    ASSERT_FALSE(stats->isDirectory());
    ASSERT_EQ(5, to<Size>(stats->get(fs::STAT_SIZE)));
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
    fs::readFile(str("hello.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    Buffer::CPtr buf = cb->getArgs()->getCPtr<Buffer>(1);
    ASSERT_TRUE(!!buf);
    ASSERT_TRUE(buf->toString()->equals(str("hello")));
}

TEST(GTestFs, TestAppendFile) {
    fs::appendFile(
        String::null(), Buffer::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::appendFile(str("hello.txt"), Buffer::create(str(" world")), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    fs::readFile(str("hello.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    Buffer::CPtr buf = cb->getArgs()->getCPtr<Buffer>(1);
    ASSERT_TRUE(!!buf);
    ASSERT_TRUE(buf->toString()->equals(str("hello world")));
}

TEST(GTestFs, TestReaddir) {
    fs::readdir(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::readdir(str("."), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    JsArray::CPtr a = cb->getArgs()->getCPtr<JsArray>(1);
    ASSERT_TRUE(a->contains(str("hello.txt")));
}

TEST(GTestFs, TestRename) {
    fs::rename(
        String::null(), String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::rename(str("hello.txt"), str("hell.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestTruncate) {
    fs::truncate(String::null(), 0, JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::truncate(str("hell.txt"), 4, cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    fs::readFile(str("hell.txt"), cb);
    node::run();
    Buffer::CPtr buf = cb->getArgs()->getCPtr<Buffer>(1);
    ASSERT_TRUE(!!buf);
    ASSERT_EQ(4, buf->length());
}

TEST(GTestFs, TestMkdir) {
    fs::mkdir(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::mkdir(str("mydir"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestSymlink) {
    fs::symlink(
        String::null(), String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::symlink(str("mydir"), str("mylink"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestLstat) {
    FsCallback::Ptr cb(new FsCallback());
    fs::lstat(str("mydir"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    fs::Stats::Ptr stats = cb->getArgs()->getPtr<fs::Stats>(1);
    ASSERT_TRUE(!!stats);
    ASSERT_TRUE(stats->isDirectory());
    ASSERT_FALSE(stats->isSocket());

    fs::lstat(str("mylink"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    stats = cb->getArgs()->getPtr<fs::Stats>(1);
    ASSERT_TRUE(!!stats);
    ASSERT_TRUE(stats->isSymbolicLink());
    ASSERT_FALSE(stats->isBlockDevice());
}

TEST(GTestFs, TestRealpath) {
    fs::realpath(String::null(), JsFunction::null());  // no crash
    node::run();

    String::CPtr current = path::resolve(JsArray::null());

    FsCallback::Ptr cb(new FsCallback());
    fs::realpath(String::null(), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    ASSERT_TRUE(cb->getArgs()->getCPtr<String>(1)->equals(current));

    fs::realpath(str("./mylink"), cb);
    String::CPtr real = current->concat(str("/mydir"));
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
    ASSERT_TRUE(cb->getArgs()->getCPtr<String>(1)->equals(real));
}

TEST(GTestFs, TestRmdir) {
    fs::rmdir(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::rmdir(str("mydir"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

TEST(GTestFs, TestUnlink) {
    fs::unlink(String::null(), JsFunction::null());  // no crash
    node::run();

    FsCallback::Ptr cb(new FsCallback());
    fs::unlink(str("hell.txt"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));

    fs::unlink(str("mylink"), cb);
    node::run();
    ASSERT_TRUE(!cb->getArgs()->getCPtr<Error>(0));
}

}  // namespace node
}  // namespace libj
