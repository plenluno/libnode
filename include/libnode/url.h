// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_URL_H_
#define LIBNODE_URL_H_

#include <libj/js_object.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace url {

extern const String::CPtr HREF;
extern const String::CPtr PROTOCOL;
extern const String::CPtr HOST;
extern const String::CPtr AUTH;
extern const String::CPtr HOSTNAME;
extern const String::CPtr PORT;
extern const String::CPtr PATHNAME;
extern const String::CPtr SEARCH;
extern const String::CPtr PATH;
extern const String::CPtr QUERY;
extern const String::CPtr HASH;

JsObject::Ptr parse(String::CPtr urlStr);
String::CPtr format(JsObject::CPtr urlObj);

}  // namespace url
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_
