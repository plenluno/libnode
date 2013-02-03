// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/path.h>

#include <libj/js_object.h>

#ifdef LIBJ_PF_WINDOWS
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

namespace libj {
namespace node {

TEST(GTestPath, TestNormalize) {
    String::CPtr null = String::null();
    String::CPtr normal = path::normalize(null);
    ASSERT_TRUE(normal->equals(String::create()));

    normal = path::normalize(String::create());
    ASSERT_TRUE(normal->equals(String::create(".")));

    normal = path::normalize(String::create("."));
    ASSERT_TRUE(normal->equals(String::create(".")));

    normal = path::normalize(String::create(".."));
    ASSERT_TRUE(normal->equals(String::create("..")));

    normal = path::normalize(String::create("/"));
    ASSERT_TRUE(normal->equals(String::create("/")));

    normal = path::normalize(String::create("//"));
    ASSERT_TRUE(normal->equals(String::create("/")));

    normal = path::normalize(String::create("x1"));
    ASSERT_TRUE(normal->equals(String::create("x1")));

    normal = path::normalize(String::create("/x2"));
    ASSERT_TRUE(normal->equals(String::create("/x2")));

    normal = path::normalize(String::create("/x3/"));
    ASSERT_TRUE(normal->equals(String::create("/x3/")));

    normal = path::normalize(String::create("/x/y"));
    ASSERT_TRUE(normal->equals(String::create("/x/y")));

    normal = path::normalize(String::create("/x/y/"));
    ASSERT_TRUE(normal->equals(String::create("/x/y/")));

    normal = path::normalize(String::create("x/y"));
    ASSERT_TRUE(normal->equals(String::create("x/y")));

    normal = path::normalize(String::create("x/y/"));
    ASSERT_TRUE(normal->equals(String::create("x/y/")));

    normal = path::normalize(String::create("x/./y"));
    ASSERT_TRUE(normal->equals(String::create("x/y")));

    normal = path::normalize(String::create("x/././y"));
    ASSERT_TRUE(normal->equals(String::create("x/y")));

    normal = path::normalize(String::create("x/./y/."));
    ASSERT_TRUE(normal->equals(String::create("x/y")));

    normal = path::normalize(String::create("x/../y"));
    ASSERT_TRUE(normal->equals(String::create("y")));

    normal = path::normalize(String::create("x/../../y"));
    ASSERT_TRUE(normal->equals(String::create("../y")));

    normal = path::normalize(String::create("x/../y/.."));
    ASSERT_TRUE(normal->equals(String::create(".")));

    normal = path::normalize(String::create("//x//.//y//..//"));
    ASSERT_TRUE(normal->equals(String::create("/x/")));

    normal = path::normalize(String::create("x/y/..."));
    ASSERT_TRUE(normal->equals(String::create("x/y/...")));
}

TEST(GTestPath, TestJoin) {
    JsArray::Ptr null = JsArray::null();
    String::CPtr joined = path::join(null);
    ASSERT_TRUE(joined->equals(String::create(".")));

    joined = path::join(JsArray::create());
    ASSERT_TRUE(joined->equals(String::create(".")));

    JsArray::Ptr paths = JsArray::create();
    paths->add(String::create("/foo"));
    paths->add(String::create("bar"));
    paths->add(String::create("baz/asdf"));
    paths->add(String::create("quux"));
    paths->add(String::create(".."));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(String::create("/foo/bar/baz/asdf")));

    paths->clear();
    paths->add(String::create("foo"));
    paths->add(JsObject::create());
    paths->add(String::create("/bar"));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(String::create("foo/bar")));
}

TEST(GTestPath, TestResolve) {
    const Size kMax = 8192;
    char dir[kMax];
    getcwd(dir, kMax);
    String::CPtr current = String::create(dir);

    JsArray::Ptr null = JsArray::null();
    String::CPtr resolved = path::resolve(null);
    ASSERT_TRUE(resolved->equals(current));

    resolved = path::resolve(JsArray::create());
    ASSERT_TRUE(resolved->equals(current));

    JsArray::Ptr paths = JsArray::create();
    paths->add(String::create("/foo/bar"));
    paths->add(String::create("./baz"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(String::create("/foo/bar/baz")));

    paths->clear();
    paths->add(String::create("/foo/bar"));
    paths->add(String::create("/tmp/file"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(String::create("/tmp/file")));

    paths->clear();
    paths->add(String::create("wwwroot"));
    paths->add(String::create("static_files/png/"));
    paths->add(String::create("../gif/image.gif"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(current->concat(
        String::create("/wwwroot/static_files/gif/image.gif"))));
}

TEST(GTestPath, TestDirname) {
    String::CPtr null = String::null();
    String::CPtr dir = path::dirname(null);
    ASSERT_TRUE(dir->equals(String::create(".")));

    dir = path::dirname(String::create("/"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("//"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("///"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("////"));
    ASSERT_TRUE(dir->equals(String::create("//")));

    dir = path::dirname(String::create("/./"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("/././"));
    ASSERT_TRUE(dir->equals(String::create("/.")));

    dir = path::dirname(String::create("x1"));
    ASSERT_TRUE(dir->equals(String::create(".")));

    dir = path::dirname(String::create("/x2"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("x3/"));
    ASSERT_TRUE(dir->equals(String::create(".")));

    dir = path::dirname(String::create("/x4/"));
    ASSERT_TRUE(dir->equals(String::create("/")));

    dir = path::dirname(String::create("/x/y"));
    ASSERT_TRUE(dir->equals(String::create("/x")));

    dir = path::dirname(String::create("x/y/"));
    ASSERT_TRUE(dir->equals(String::create("x")));
}

TEST(GTestPath, TestBasename) {
    String::CPtr null = String::null();
    String::CPtr base = path::basename(null);
    ASSERT_TRUE(base->equals(String::create()));

    base = path::basename(String::create("/"));
    ASSERT_TRUE(base->equals(String::create()));

    base = path::basename(String::create("//"));
    ASSERT_TRUE(base->equals(String::create()));

    base = path::basename(String::create("///"));
    ASSERT_TRUE(base->equals(String::create("/")));

    base = path::basename(String::create("/./"));
    ASSERT_TRUE(base->equals(String::create("./")));

    base = path::basename(String::create("x1"));
    ASSERT_TRUE(base->equals(String::create("x1")));

    base = path::basename(String::create("/x2"));
    ASSERT_TRUE(base->equals(String::create("x2")));

    base = path::basename(String::create("/x3/"));
    ASSERT_TRUE(base->equals(String::create("x3/")));

    base = path::basename(String::create("x/y"));
    ASSERT_TRUE(base->equals(String::create("y")));

    base = path::basename(String::create("/x/y/"));
    ASSERT_TRUE(base->equals(String::create("y/")));
}

TEST(GTestPath, TestExtname) {
    String::CPtr null = String::null();
    String::CPtr ext = path::extname(null);
    ASSERT_TRUE(ext->equals(String::create()));

    ext = path::extname(String::create("."));
    ASSERT_TRUE(ext->equals(String::create()));

    ext = path::extname(String::create("index"));
    ASSERT_TRUE(ext->equals(String::create()));

    ext = path::extname(String::create("index."));
    ASSERT_TRUE(ext->equals(String::create(".")));

    ext = path::extname(String::create(".html"));
    ASSERT_TRUE(ext->equals(String::create(".html")));

    ext = path::extname(String::create("index.html"));
    ASSERT_TRUE(ext->equals(String::create(".html")));

    ext = path::extname(String::create("/xyz/index.html"));
    ASSERT_TRUE(ext->equals(String::create(".html")));

    ext = path::extname(String::create("/xyz/index.html/"));
    ASSERT_TRUE(ext->equals(String::create()));
}

}  // namespace node
}  // namespace libj
