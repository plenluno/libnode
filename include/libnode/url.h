// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_URL_H_
#define LIBNODE_URL_H_

#include "libj/string.h"
#include "libj/js_object.h"

namespace libj {
namespace node {
namespace url {

extern String::CPtr HREF;
extern String::CPtr PROTOCOL;
extern String::CPtr HOST;
extern String::CPtr AUTH;
extern String::CPtr HOSTNAME;
extern String::CPtr PORT;
extern String::CPtr PATHNAME;
extern String::CPtr SEARCH;
extern String::CPtr PATH;
extern String::CPtr QUERY;
extern String::CPtr HASH;

JsObject::Ptr parse(String::CPtr urlStr);
String::CPtr format(JsObject::CPtr urlObj);

}  // namespace url
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_