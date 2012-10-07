// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_PROCESS_H_
#define LIBNODE_PROCESS_H_

#include <libj/js_function.h>

namespace libj {
namespace node {
namespace process {

void nextTick(JsFunction::Ptr callback);

}  // namespace process
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_PROCESS_H_
