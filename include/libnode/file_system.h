// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_FILE_SYSTEM_H_
#define LIBNODE_FILE_SYSTEM_H_

#include <libj/js_function.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace fs {

void readFile(String::CPtr fileName, JsFunction::Ptr callback);

}  // namespace fs
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_FILE_SYSTEM_H_
