// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_INVOKE_H_
#define LIBNODE_IMPL_INVOKE_H_

#include <libnode/detail/arguments_list.h>

namespace libj {
namespace node {

inline Value invoke(JsFunction::Ptr func) {
    return (*func)();
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    args->add(arg5);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    args->add(arg5);
    args->add(arg6);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    args->add(arg5);
    args->add(arg6);
    args->add(arg7);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7, const Value& arg8) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    args->add(arg5);
    args->add(arg6);
    args->add(arg7);
    args->add(arg8);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

inline Value invoke(
    JsFunction::Ptr func,
    const Value& arg1, const Value& arg2, const Value& arg3,
    const Value& arg4, const Value& arg5, const Value& arg6,
    const Value& arg7, const Value& arg8, const Value& arg9) {
    LIBNODE_ARGUMENTS_ALLOC(args);
    args->add(arg1);
    args->add(arg2);
    args->add(arg3);
    args->add(arg4);
    args->add(arg5);
    args->add(arg6);
    args->add(arg7);
    args->add(arg8);
    args->add(arg9);
    Value res = (*func)(args);
    LIBNODE_ARGUMENTS_FREE(args);
    return res;
}

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_IMPL_INVOKE_H_
