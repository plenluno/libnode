// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <unistd.h>
#include <libj/js_object.h>
#include <libnode/path.h>

namespace libj {
namespace node {

TEST(GTestPath, TestNormalize) {
    String::CPtr null = String::null();
    String::CPtr normal = path::normalize(null);
    ASSERT_EQ(normal->compareTo(String::create()), 0);

    normal = path::normalize(String::create());
    ASSERT_EQ(normal->compareTo(String::create(".")), 0);

    normal = path::normalize(String::create("."));
    ASSERT_EQ(normal->compareTo(String::create(".")), 0);

    normal = path::normalize(String::create(".."));
    ASSERT_EQ(normal->compareTo(String::create("..")), 0);

    normal = path::normalize(String::create("/"));
    ASSERT_EQ(normal->compareTo(String::create("/")), 0);

    normal = path::normalize(String::create("//"));
    ASSERT_EQ(normal->compareTo(String::create("/")), 0);

    normal = path::normalize(String::create("x1"));
    ASSERT_EQ(normal->compareTo(String::create("x1")), 0);

    normal = path::normalize(String::create("/x2"));
    ASSERT_EQ(normal->compareTo(String::create("/x2")), 0);

    normal = path::normalize(String::create("/x3/"));
    ASSERT_EQ(normal->compareTo(String::create("/x3/")), 0);

    normal = path::normalize(String::create("/x/y"));
    ASSERT_EQ(normal->compareTo(String::create("/x/y")), 0);

    normal = path::normalize(String::create("/x/y/"));
    ASSERT_EQ(normal->compareTo(String::create("/x/y/")), 0);

    normal = path::normalize(String::create("x/y"));
    ASSERT_EQ(normal->compareTo(String::create("x/y")), 0);

    normal = path::normalize(String::create("x/y/"));
    ASSERT_EQ(normal->compareTo(String::create("x/y/")), 0);

    normal = path::normalize(String::create("x/./y"));
    ASSERT_EQ(normal->compareTo(String::create("x/y")), 0);

    normal = path::normalize(String::create("x/././y"));
    ASSERT_EQ(normal->compareTo(String::create("x/y")), 0);

    normal = path::normalize(String::create("x/./y/."));
    ASSERT_EQ(normal->compareTo(String::create("x/y")), 0);

    normal = path::normalize(String::create("x/../y"));
    ASSERT_EQ(normal->compareTo(String::create("y")), 0);

    normal = path::normalize(String::create("x/../../y"));
    ASSERT_EQ(normal->compareTo(String::create("../y")), 0);

    normal = path::normalize(String::create("x/../y/.."));
    ASSERT_EQ(normal->compareTo(String::create(".")), 0);

    normal = path::normalize(String::create("//x//.//y//..//"));
    ASSERT_EQ(normal->compareTo(String::create("/x/")), 0);

    normal = path::normalize(String::create("x/y/..."));
    ASSERT_EQ(normal->compareTo(String::create("x/y/...")), 0);
}

TEST(GTestPath, TestJoin) {
    JsArray::Ptr null = JsArray::null();
    String::CPtr joined = path::join(null);
    ASSERT_EQ(joined->compareTo(String::create(".")), 0);

    joined = path::join(JsArray::create());
    ASSERT_EQ(joined->compareTo(String::create(".")), 0);

    JsArray::Ptr paths = JsArray::create();
    paths->add(String::create("/foo"));
    paths->add(String::create("bar"));
    paths->add(String::create("baz/asdf"));
    paths->add(String::create("quux"));
    paths->add(String::create(".."));
    joined = path::join(paths);
    ASSERT_EQ(joined->compareTo(String::create("/foo/bar/baz/asdf")), 0);

    paths->clear();
    paths->add(String::create("foo"));
    paths->add(JsObject::create());
    paths->add(String::create("/bar"));
    joined = path::join(paths);
    ASSERT_EQ(joined->compareTo(String::create("foo/bar")), 0);
}

TEST(GTestPath, TestResolve) {
    const Size kMax = 8192;
    char dir[kMax];
    getcwd(dir, kMax);
    String::CPtr current = String::create(dir);

    JsArray::Ptr null = JsArray::null();
    String::CPtr resolved = path::resolve(null);
    ASSERT_EQ(resolved->compareTo(current), 0);

    resolved = path::resolve(JsArray::create());
    ASSERT_EQ(resolved->compareTo(current), 0);

    JsArray::Ptr paths = JsArray::create();
    paths->add(String::create("/foo/bar"));
    paths->add(String::create("./baz"));
    resolved = path::resolve(paths);
    ASSERT_EQ(resolved->compareTo(String::create("/foo/bar/baz")), 0);

    paths->clear();
    paths->add(String::create("/foo/bar"));
    paths->add(String::create("/tmp/file"));
    resolved = path::resolve(paths);
    ASSERT_EQ(resolved->compareTo(String::create("/tmp/file")), 0);

    paths->clear();
    paths->add(String::create("wwwroot"));
    paths->add(String::create("static_files/png/"));
    paths->add(String::create("../gif/image.gif"));
    resolved = path::resolve(paths);
    ASSERT_EQ(resolved->compareTo(current->concat(
        String::create("/wwwroot/static_files/gif/image.gif"))), 0);
}

TEST(GTestPath, TestDirname) {
    String::CPtr null = String::null();
    String::CPtr dir = path::dirname(null);
    ASSERT_EQ(dir->compareTo(String::create(".")), 0);

    dir = path::dirname(String::create("/"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("//"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("///"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("////"));
    ASSERT_EQ(dir->compareTo(String::create("//")), 0);

    dir = path::dirname(String::create("/./"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("/././"));
    ASSERT_EQ(dir->compareTo(String::create("/.")), 0);

    dir = path::dirname(String::create("x1"));
    ASSERT_EQ(dir->compareTo(String::create(".")), 0);

    dir = path::dirname(String::create("/x2"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("x3/"));
    ASSERT_EQ(dir->compareTo(String::create(".")), 0);

    dir = path::dirname(String::create("/x4/"));
    ASSERT_EQ(dir->compareTo(String::create("/")), 0);

    dir = path::dirname(String::create("/x/y"));
    ASSERT_EQ(dir->compareTo(String::create("/x")), 0);

    dir = path::dirname(String::create("x/y/"));
    ASSERT_EQ(dir->compareTo(String::create("x")), 0);
}

TEST(GTestPath, TestBasename) {
    String::CPtr null = String::null();
    String::CPtr base = path::basename(null);
    ASSERT_EQ(base->compareTo(String::create()), 0);

    base = path::basename(String::create("/"));
    ASSERT_EQ(base->compareTo(String::create()), 0);

    base = path::basename(String::create("//"));
    ASSERT_EQ(base->compareTo(String::create()), 0);

    base = path::basename(String::create("///"));
    ASSERT_EQ(base->compareTo(String::create("/")), 0);

    base = path::basename(String::create("/./"));
    ASSERT_EQ(base->compareTo(String::create("./")), 0);

    base = path::basename(String::create("x1"));
    ASSERT_EQ(base->compareTo(String::create("x1")), 0);

    base = path::basename(String::create("/x2"));
    ASSERT_EQ(base->compareTo(String::create("x2")), 0);

    base = path::basename(String::create("/x3/"));
    ASSERT_EQ(base->compareTo(String::create("x3/")), 0);

    base = path::basename(String::create("x/y"));
    ASSERT_EQ(base->compareTo(String::create("y")), 0);

    base = path::basename(String::create("/x/y/"));
    ASSERT_EQ(base->compareTo(String::create("y/")), 0);
}

TEST(GTestPath, TestExtname) {
    String::CPtr null = String::null();
    String::CPtr ext = path::extname(null);
    ASSERT_EQ(ext->compareTo(String::create()), 0);

    ext = path::extname(String::create("."));
    ASSERT_EQ(ext->compareTo(String::create()), 0);

    ext = path::extname(String::create("index"));
    ASSERT_EQ(ext->compareTo(String::create()), 0);

    ext = path::extname(String::create("index."));
    ASSERT_EQ(ext->compareTo(String::create(".")), 0);

    ext = path::extname(String::create(".html"));
    ASSERT_EQ(ext->compareTo(String::create(".html")), 0);

    ext = path::extname(String::create("index.html"));
    ASSERT_EQ(ext->compareTo(String::create(".html")), 0);

    ext = path::extname(String::create("/xyz/index.html"));
    ASSERT_EQ(ext->compareTo(String::create(".html")), 0);

    ext = path::extname(String::create("/xyz/index.html/"));
    ASSERT_EQ(ext->compareTo(String::create()), 0);
}

}  // namespace node
}  // namespace libj
