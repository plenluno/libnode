// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_TIMER_H_
#define LIBNODE_TIMER_H_

#include "libj/js_array.h"
#include "libj/js_function.h"

namespace libj {
namespace node {

Value setTimeout(JsFunction::Ptr callback, Int delay, JsArray::CPtr args);
Value setInterval(JsFunction::Ptr callback, Int delay, JsArray::CPtr args);
void clearTimeout(Value timeoutId);
void clearInterval(Value intervalId);

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_TIMER_H_