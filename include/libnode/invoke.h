// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_INVOKE_H_
#define LIBNODE_INVOKE_H_

#include <libj/js_function.h>

namespace libj {
namespace node {

Value invoke(JsFunction::Ptr func);

Value invoke(JsFunction::Ptr func, const Value& arg1);

Value invoke(JsFunction::Ptr func, const Value& arg1, const Value& arg2);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7, const Value& arg8);

Value invoke(JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7, const Value& arg8, const Value& arg9);

}  // namespace node
}  // namespace libj

#include <libnode/impl/invoke.h>

#endif  // LIBNODE_INVOKE_H_
