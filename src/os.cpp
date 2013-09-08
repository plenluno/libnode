// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/os.h>
#include <libnode/detail/os.h>

#include <libj/endian.h>

#ifdef LIBJ_PF_WINDOWS
# define MAXHOSTNAMELEN 256
#else
# include <sys/param.h>
# include <sys/utsname.h>
#endif

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
#ifdef LIBJ_PF_WINDOWS
    LIBJ_STATIC_SYMBOL_DEF(symType, "Windows_NT");

    return symType;
#else
    struct utsname info;
    if (uname(&info) < 0) {
        return String::null();
    } else {
        return String::create(info.sysname);
    }
#endif
}

String::CPtr release() {
#ifdef LIBJ_PF_WINDOWS
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    if (!GetVersionEx(&info)) return String::null();

    StringBuilder::Ptr sb = StringBuilder::create();
    sb->append(static_cast<ULong>(info.dwMajorVersion));
    sb->appendChar('.');
    sb->append(static_cast<ULong>(info.dwMinorVersion));
    sb->appendChar('.');
    sb->append(static_cast<ULong>(info.dwBuildNumber));
    return sb->toString();
#else
    struct utsname info;
    if (uname(&info) < 0) {
        return String::null();
    } else {
        return String::create(info.release);
    }
#endif
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
