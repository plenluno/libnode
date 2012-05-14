// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_JS_OBJECT_H_
#define LIBNODE_JS_OBJECT_H_

#include "libj/map.h"
#include "libj/null.h"

namespace libj {
namespace node {

class JsObject : LIBJ_MAP(JsObject)
};

#define LIBNODE_JS_OBJECT(T) public libj::node::JsObject { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsObject)

#define LIBNODE_JS_OBJECT_WITHOUT_CREATE(T) public libj::node::JsObject { \
    LIBJ_MUTABLE_DECLS_WITHOUT_CREATE(T, libj::node::JsObject)

#define LIBNODE_JS_OBJECT_IMPL(JO) \
    LIBJ_MAP_IMPL(JO) \
 public: \
    String::CPtr toString() const { \
        return JO->toString(); \
    } \
    template<typename T> \
    typename Type<T>::Ptr getPtr(String::CPtr name) const { \
        Value v = get(name); \
        if (v.instanceOf(Type<Null>::id())) { \
            LIBJ_NULL_PTR_TYPE(T, nullp); \
            return nullp; \
        } \
        typename Type<T>::Ptr p = toPtr<T>(v); \
        return p; \
    } \
    template<typename T> \
    typename Type<T>::CPtr getCPtr(String::CPtr name) const { \
        Value v = get(name); \
        if (v.instanceOf(Type<Null>::id())) { \
            LIBJ_NULL_CPTR_TYPE(T, nullp); \
            return nullp; \
        } \
        typename Type<T>::CPtr p = toCPtr<T>(v); \
        return p; \
    }
    

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENT_EMITTER_H_