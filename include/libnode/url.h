// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_URL_H_
#define LIBNODE_URL_H_

#include "libj/string.h"
#include "libnode/js_object.h"

namespace libj {
namespace node {
namespace url {

Type<JsObject>::Ptr parse(Type<String>::Cptr urlStr);
Type<String>::Cptr format(Type<JsObject>::Cptr urlObj);

}  // namespace url
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_