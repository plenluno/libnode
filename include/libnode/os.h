// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_OS_H_
#define LIBNODE_OS_H_

#include <libj/js_object.h>

namespace libj {
namespace node {
namespace os {

String::CPtr endianness();

String::CPtr hostname();

String::CPtr type();

String::CPtr release();

JsObject::Ptr networkInterface();

String::CPtr EOL();

}  // namespace os
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_OS_H_
