// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/path.h>

#include <libj/js_object.h>

#ifdef LIBJ_PF_WINDOWS
# include <libj/platform/windows.h>
#else
# include <unistd.h>
#endif

namespace libj {
namespace node {

TEST(GTestPath, TestNormalize) {
    String::CPtr null = String::null();
    String::CPtr normal = path::normalize(null);
    ASSERT_TRUE(normal->equals(str()));

    normal = path::normalize(str());
    ASSERT_TRUE(normal->equals(str(".")));

    normal = path::normalize(str("."));
    ASSERT_TRUE(normal->equals(str(".")));

    normal = path::normalize(str(".."));
    ASSERT_TRUE(normal->equals(str("..")));

    normal = path::normalize(str("/"));
    ASSERT_TRUE(normal->equals(str("/")));

    normal = path::normalize(str("//"));
    ASSERT_TRUE(normal->equals(str("/")));

    normal = path::normalize(str("x1"));
    ASSERT_TRUE(normal->equals(str("x1")));

    normal = path::normalize(str("/x2"));
    ASSERT_TRUE(normal->equals(str("/x2")));

    normal = path::normalize(str("/x3/"));
    ASSERT_TRUE(normal->equals(str("/x3/")));

    normal = path::normalize(str("/x/y"));
    ASSERT_TRUE(normal->equals(str("/x/y")));

    normal = path::normalize(str("/x/y/"));
    ASSERT_TRUE(normal->equals(str("/x/y/")));

    normal = path::normalize(str("x/y"));
    ASSERT_TRUE(normal->equals(str("x/y")));

    normal = path::normalize(str("x/y/"));
    ASSERT_TRUE(normal->equals(str("x/y/")));

    normal = path::normalize(str("x/./y"));
    ASSERT_TRUE(normal->equals(str("x/y")));

    normal = path::normalize(str("x/././y"));
    ASSERT_TRUE(normal->equals(str("x/y")));

    normal = path::normalize(str("x/./y/."));
    ASSERT_TRUE(normal->equals(str("x/y")));

    normal = path::normalize(str("x/../y"));
    ASSERT_TRUE(normal->equals(str("y")));

    normal = path::normalize(str("x/../../y"));
    ASSERT_TRUE(normal->equals(str("../y")));

    normal = path::normalize(str("x/../y/.."));
    ASSERT_TRUE(normal->equals(str(".")));

    normal = path::normalize(str("//x//.//y//..//"));
    ASSERT_TRUE(normal->equals(str("/x/")));

    normal = path::normalize(str("x/y/..."));
    ASSERT_TRUE(normal->equals(str("x/y/...")));
}

TEST(GTestPath, TestJoin) {
    JsArray::Ptr null = JsArray::null();
    String::CPtr joined = path::join(null);
    ASSERT_TRUE(joined->equals(str(".")));

    joined = path::join(JsArray::create());
    ASSERT_TRUE(joined->equals(str(".")));

    JsArray::Ptr paths = JsArray::create();
    paths->add(str("/foo"));
    paths->add(str("bar"));
    paths->add(str("baz/asdf"));
    paths->add(str("quux"));
    paths->add(str(".."));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(str("/foo/bar/baz/asdf")));

    paths->clear();
    paths->add(str("foo"));
    paths->add(JsObject::create());
    paths->add(str("/bar"));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(str("foo/bar")));
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
    paths->add(String::null());
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(current));

    paths->clear();
    paths->add(str());
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(current));

    paths->clear();
    paths->add(str("/foo/bar"));
    paths->add(str("./baz"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(str("/foo/bar/baz")));

    paths->clear();
    paths->add(str("/foo/bar"));
    paths->add(str("/tmp/file"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(str("/tmp/file")));

    paths->clear();
    paths->add(str("wwwroot"));
    paths->add(str("static_files/png/"));
    paths->add(str("../gif/image.gif"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(current->concat(
        str("/wwwroot/static_files/gif/image.gif"))));
}

TEST(GTestPath, TestDirname) {
    String::CPtr null = String::null();
    String::CPtr dir = path::dirname(null);
    ASSERT_TRUE(dir->equals(str(".")));

    dir = path::dirname(str("/"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("//"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("///"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("////"));
    ASSERT_TRUE(dir->equals(str("//")));

    dir = path::dirname(str("/./"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("/././"));
    ASSERT_TRUE(dir->equals(str("/.")));

    dir = path::dirname(str("x1"));
    ASSERT_TRUE(dir->equals(str(".")));

    dir = path::dirname(str("/x2"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("x3/"));
    ASSERT_TRUE(dir->equals(str(".")));

    dir = path::dirname(str("/x4/"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("/x/y"));
    ASSERT_TRUE(dir->equals(str("/x")));

    dir = path::dirname(str("x/y/"));
    ASSERT_TRUE(dir->equals(str("x")));
}

TEST(GTestPath, TestBasename) {
    String::CPtr null = String::null();
    String::CPtr base = path::basename(null);
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("/"));
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("//"));
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("///"));
    ASSERT_TRUE(base->equals(str("/")));

    base = path::basename(str("/./"));
    ASSERT_TRUE(base->equals(str("./")));

    base = path::basename(str("x1"));
    ASSERT_TRUE(base->equals(str("x1")));

    base = path::basename(str("/x2"));
    ASSERT_TRUE(base->equals(str("x2")));

    base = path::basename(str("/x3/"));
    ASSERT_TRUE(base->equals(str("x3/")));

    base = path::basename(str("x/y"));
    ASSERT_TRUE(base->equals(str("y")));

    base = path::basename(str("/x/y/"));
    ASSERT_TRUE(base->equals(str("y/")));
}

TEST(GTestPath, TestExtname) {
    String::CPtr null = String::null();
    String::CPtr ext = path::extname(null);
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("."));
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("index"));
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("index."));
    ASSERT_TRUE(ext->equals(str(".")));

    ext = path::extname(str(".html"));
    ASSERT_TRUE(ext->equals(str(".html")));

    ext = path::extname(str("index.html"));
    ASSERT_TRUE(ext->equals(str(".html")));

    ext = path::extname(str("/xyz/index.html"));
    ASSERT_TRUE(ext->equals(str(".html")));

    ext = path::extname(str("/xyz/index.html/"));
    ASSERT_TRUE(ext->equals(str()));
}

}  // namespace node
}  // namespace libj
