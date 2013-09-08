// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/os.h>
#include <libnode/detail/os.h>

#include <libj/endian.h>

#include <sys/param.h>
#include <sys/utsname.h>

namespace libj {
namespace node {
namespace os {

String::CPtr endianness() {
    LIBJ_STATIC_SYMBOL_DEF(symBE, "BE");
    LIBJ_STATIC_SYMBOL_DEF(symLE, "LE");

    return endian() == BIG ? symBE : symLE;
}

String::CPtr hostname() {
    char buf[MAXHOSTNAMELEN + 1];
    if (gethostname(buf, sizeof(buf))) {
        return String::null();
    } else {
        buf[sizeof(buf) - 1] = 0;
        return String::create(buf);
    }
}

String::CPtr type() {
    struct utsname info;
    if (uname(&info) < 0) {
        return String::null();
    } else {
        return String::create(info.sysname);
    }
}

String::CPtr release() {
    struct utsname info;
    if (uname(&info) < 0) {
        return String::null();
    } else {
        return String::create(info.release);
    }
}

JsObject::Ptr networkInterface() {
    return node::detail::os::networkInterface();
}

String::CPtr EOL() {
#ifdef LIBJ_PF_WINDOWS
    LIBJ_STATIC_SYMBOL_DEF(symEOL, "\r\n");
#else
    LIBJ_STATIC_SYMBOL_DEF(symEOL, "\n");
#endif

    return symEOL;
}

}  // namespace os
}  // namespace node
}  // namespace libj
