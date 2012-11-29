// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_UTIL_H_
#define LIBNODE_UTIL_H_

#include <libj/string.h>
#include <libnode/buffer.h>

namespace libj {
namespace node {
namespace util {

Boolean isArray(const Value& val);
Boolean isError(const Value& val);
Boolean isRegExp(const Value& val);

Boolean extend(JsObject::Ptr derived, JsObject::CPtr super);

String::CPtr hexEncode(Buffer::CPtr buf);
String::CPtr hexEncode(const void* data, Size length);
Buffer::Ptr hexDecode(String::CPtr str);

String::CPtr base64Encode(Buffer::CPtr buf);
String::CPtr base64Encode(const void* data, Size length);
Buffer::Ptr base64Decode(String::CPtr str);

String::CPtr percentEncode(
    String::CPtr str, String::Encoding enc = String::UTF8);
String::CPtr percentDecode(
    String::CPtr str, String::Encoding enc = String::UTF8);

}  // namespace util
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_UTIL_H_
