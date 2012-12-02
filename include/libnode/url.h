// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_URL_H_
#define LIBNODE_URL_H_

#include <libj/js_object.h>
#include <libj/symbol.h>

namespace libj {
namespace node {
namespace url {

extern Symbol::CPtr HREF;
extern Symbol::CPtr PROTOCOL;
extern Symbol::CPtr HOST;
extern Symbol::CPtr AUTH;
extern Symbol::CPtr HOSTNAME;
extern Symbol::CPtr PORT;
extern Symbol::CPtr PATHNAME;
extern Symbol::CPtr SEARCH;
extern Symbol::CPtr PATH;
extern Symbol::CPtr QUERY;
extern Symbol::CPtr HASH;

JsObject::Ptr parse(String::CPtr urlStr);

String::CPtr format(JsObject::CPtr urlObj);

}  // namespace url
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_
