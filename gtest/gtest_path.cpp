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

static inline String::CPtr p(const char* s = NULL) {
    String::CPtr path = s ? str(s) : str();
    return path->replace(str("/"), path::sep());
}

TEST(GTestPath, TestNormalize) {
    String::CPtr normal = path::normalize(String::null());
    ASSERT_TRUE(normal->equals(p("null")));

    normal = path::normalize(str());
    ASSERT_TRUE(normal->equals(p(".")));

    normal = path::normalize(str("."));
    ASSERT_TRUE(normal->equals(p(".")));

    normal = path::normalize(str(".."));
    ASSERT_TRUE(normal->equals(p("..")));

    normal = path::normalize(str("/"));
    ASSERT_TRUE(normal->equals(p("/")));

    normal = path::normalize(str("//"));
    ASSERT_TRUE(normal->equals(p("/")));

    normal = path::normalize(str("x1"));
    ASSERT_TRUE(normal->equals(p("x1")));

    normal = path::normalize(str("/x2"));
    ASSERT_TRUE(normal->equals(p("/x2")));

    normal = path::normalize(str("/x3/"));
    ASSERT_TRUE(normal->equals(p("/x3/")));

    normal = path::normalize(str("/x/y"));
    ASSERT_TRUE(normal->equals(p("/x/y")));

    normal = path::normalize(str("/x/y/"));
    ASSERT_TRUE(normal->equals(p("/x/y/")));

    normal = path::normalize(str("x/y"));
    ASSERT_TRUE(normal->equals(p("x/y")));

    normal = path::normalize(str("x/y/"));
    ASSERT_TRUE(normal->equals(p("x/y/")));

    normal = path::normalize(str("x/./y"));
    ASSERT_TRUE(normal->equals(p("x/y")));

    normal = path::normalize(str("x/././y"));
    ASSERT_TRUE(normal->equals(p("x/y")));

    normal = path::normalize(str("x/./y/."));
    ASSERT_TRUE(normal->equals(p("x/y")));

    normal = path::normalize(str("x/../y"));
    ASSERT_TRUE(normal->equals(p("y")));

    normal = path::normalize(str("x/../../y"));
    ASSERT_TRUE(normal->equals(p("../y")));

    normal = path::normalize(str("x/../y/.."));
    ASSERT_TRUE(normal->equals(p(".")));

    normal = path::normalize(str("//x//.//y//..//"));
    ASSERT_TRUE(normal->equals(p("/x/")));

    normal = path::normalize(str("x/y/..."));
    ASSERT_TRUE(normal->equals(p("x/y/...")));

#ifdef LIBJ_PF_WINDOWS
    normal = path::normalize(str("c:\\x\\Y"));
    ASSERT_TRUE(normal->equals(p("c:\\x\\Y")));
#endif
}

TEST(GTestPath, TestJoin) {
    String::CPtr joined = path::join(JsArray::null());
    ASSERT_TRUE(joined->equals(p(".")));

    joined = path::join(JsArray::create());
    ASSERT_TRUE(joined->equals(p(".")));

    JsArray::Ptr paths = JsArray::create();
    paths->add(str("/foo"));
    paths->add(str("bar"));
    paths->add(str("baz/asdf"));
    paths->add(str("quux"));
    paths->add(str(".."));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(p("/foo/bar/baz/asdf")));

    paths->clear();
    paths->add(str("foo"));
    paths->add(JsObject::create());
    paths->add(str("/bar"));
    joined = path::join(paths);
    ASSERT_TRUE(joined->equals(p("foo/bar")));
}

TEST(GTestPath, TestResolve) {
    const Size kMax = 8192;
    char dir[kMax];
    ASSERT_TRUE(getcwd(dir, kMax));
    String::CPtr current = str(dir);
#ifdef LIBJ_PF_WINDOWS
    String::CPtr drive = current->substring(0, 2);
#else
    String::CPtr drive = str();
#endif

    String::CPtr resolved = path::resolve(JsArray::null());
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
    ASSERT_TRUE(resolved->equals(drive->concat(p("/foo/bar/baz"))));

    paths->clear();
    paths->add(str("/foo/bar"));
    paths->add(str("/tmp/file"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(drive->concat(p("/tmp/file"))));

    paths->clear();
    paths->add(str("wwwroot"));
    paths->add(str("static_files/png/"));
    paths->add(str("../gif/image.gif"));
    resolved = path::resolve(paths);
    ASSERT_TRUE(resolved->equals(current->concat(
        p("/wwwroot/static_files/gif/image.gif"))));
}

TEST(GTestPath, TestDirname) {
    String::CPtr dir = path::dirname(String::null());
    ASSERT_TRUE(dir->equals(str(".")));

    dir = path::dirname(str());
    ASSERT_TRUE(dir->equals(str(".")));

    dir = path::dirname(str("/"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("//"));
    ASSERT_TRUE(dir->equals(str("/")));

    dir = path::dirname(str("///"));
    ASSERT_TRUE(dir->equals(str("/")));

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
    String::CPtr base = path::basename(String::null());
    ASSERT_TRUE(base->equals(str("null")));

    base = path::basename(str());
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("/"));
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("//"));
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("///"));
    ASSERT_TRUE(base->equals(str()));

    base = path::basename(str("."));
    ASSERT_TRUE(base->equals(str(".")));

    base = path::basename(str(".."));
    ASSERT_TRUE(base->equals(str("..")));

    base = path::basename(str("/./"));
    ASSERT_TRUE(base->equals(str(".")));

    base = path::basename(str("/../"));
    ASSERT_TRUE(base->equals(str("..")));

    base = path::basename(str("x1"));
    ASSERT_TRUE(base->equals(str("x1")));

    base = path::basename(str("/x2"));
    ASSERT_TRUE(base->equals(str("x2")));

    base = path::basename(str("/x3/"));
    ASSERT_TRUE(base->equals(str("x3")));

    base = path::basename(str("x/y"));
    ASSERT_TRUE(base->equals(str("y")));

    base = path::basename(str("/x/y/"));
    ASSERT_TRUE(base->equals(str("y")));
}

TEST(GTestPath, TestExtname) {
    String::CPtr ext = path::extname(String::null());
    ASSERT_TRUE(ext->equals(p()));

    ext = path::extname(str());
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("."));
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("index"));
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("index."));
    ASSERT_TRUE(ext->equals(str(".")));

    ext = path::extname(str(".html"));
    ASSERT_TRUE(ext->equals(str()));

    ext = path::extname(str("index.html"));
    ASSERT_TRUE(ext->equals(str(".html")));

    ext = path::extname(str("/xyz/index.html"));
    ASSERT_TRUE(ext->equals(str(".html")));

    ext = path::extname(str("/xyz/index.html/"));
    ASSERT_TRUE(ext->equals(str(".html")));
}

}  // namespace node
}  // namespace libj
