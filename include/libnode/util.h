// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBNODE_UTIL_H_
#define LIBNODE_UTIL_H_

#include <libnode/buffer.h>

#include <libj/string.h>

namespace libj {
namespace node {
namespace util {

Boolean isArray(const Value& val);

Boolean isError(const Value& val);

Boolean isRegExp(const Value& val);

JsObject::Ptr extend(JsObject::Ptr target, JsObject::CPtr addition);

String::CPtr hexEncode(Buffer::CPtr buf);

String::CPtr hexEncode(const void* data, Size length);

Buffer::Ptr hexDecode(String::CPtr str);

Buffer::Ptr hexDecode(StringBuilder::CPtr sb);

String::CPtr base64Encode(Buffer::CPtr buf);

String::CPtr base64Encode(const void* data, Size length);

Buffer::Ptr base64Decode(String::CPtr str);

Buffer::Ptr base64Decode(StringBuilder::CPtr sb);

String::CPtr percentEncode(
    String::CPtr str, String::Encoding enc = String::UTF8);

String::CPtr percentDecode(
    String::CPtr str, String::Encoding enc = String::UTF8);

}  // namespace util
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_UTIL_H_
