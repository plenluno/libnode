// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_JS_OBJECT_H_
#define LIBNODE_JS_OBJECT_H_

#include "libj/map.h"
#include "libj/null.h"

namespace libj {
namespace node {

class JsObject : LIBJ_MAP(JsObject)
 public:
    template<typename T>
    typename Type<T>::Ptr getPtr(String::CPtr name) const {
        Value v = get(name);
        typename Type<T>::Ptr p = toPtr<T>(v);
        return p;
    }
    
    template<typename T>
    typename Type<T>::CPtr getCPtr(String::CPtr name) const {
        Value v = get(name);
        typename Type<T>::CPtr p = toCPtr<T>(v);
        return p;
    }
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
    }    

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_EVENT_EMITTER_H_