// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_UTIL_H_
#define LIBNODE_UTIL_H_

#include <libj/string.h>

namespace libj {
namespace node {
namespace util {

String::CPtr percentEncode(
    String::CPtr str, String::Encoding enc = String::UTF8);
String::CPtr percentDecode(
    String::CPtr str, String::Encoding enc = String::UTF8);

}  // namespace util
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_UTIL_H_
