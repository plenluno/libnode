// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_ARGUMENTS_LIST_H_
#define LIBNODE_DETAIL_ARGUMENTS_LIST_H_

#include <libnode/config.h>
#include <libnode/detail/free_list.h>

#include <libj/js_array.h>

namespace libj {
namespace node {
namespace detail {

inline FreeList<JsArray::Ptr>* argumentsList() {
#ifdef LIBNODE_USE_SP
    static FreeList<JsArray::Ptr> list(100);
#else
    static FreeList<JsArray::Ptr> list(0);
#endif
    return &list;
}

}  // namespace detail
}  // namespace node
}  // namespace libj

#ifdef LIBNODE_USE_SP

#define LIBNODE_ARGUMENTS_ALLOC(ARGS) \
    libj::JsArray::Ptr ARGS = libj::node::detail::argumentsList()->alloc(); \
    if (ARGS) { \
        ARGS->clear(); \
    } else { \
        ARGS = libj::JsArray::create(); \
    }

#define LIBNODE_ARGUMENTS_FREE(ARGS) \
    if (ARGS.use_count() == 1) { \
        libj::node::detail::argumentsList()->free(ARGS); \
    }

#else  // LIBNODE_USE_SP

#define LIBNODE_ARGUMENTS_ALLOC(ARGS) \
    libj::JsArray::Ptr ARGS = libj::JsArray::create();

#define LIBNODE_ARGUMENTS_FREE(ARGS)

#endif  // LIBNODE_USE_SP

#endif  // LIBNODE_DETAIL_ARGUMENTS_LIST_H_
