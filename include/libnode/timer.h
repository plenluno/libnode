// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_TIMER_H_
#define LIBNODE_TIMER_H_

#include <libj/js_array.h>
#include <libj/js_function.h>

namespace libj {
namespace node {

Value setTimeout(
	JsFunction::Ptr callback,
	Int delay,
	JsArray::Ptr args = JsArray::null());

Value setInterval(
	JsFunction::Ptr callback,
	Int delay,
	JsArray::Ptr args = JsArray::null());

void clearTimeout(Value timeoutId);

void clearInterval(Value intervalId);

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_TIMER_H_
