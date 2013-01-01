// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_BUFFER_H_
#define LIBNODE_IMPL_BUFFER_H_

#include <libnode/util.h>

#include <libj/exception.h>

namespace libj {
namespace node {

inline Boolean Buffer::isBuffer(const Value& val) {
    return val.instanceof(Type<Buffer>::id());
}

inline Size Buffer::byteLength(String::CPtr str, Encoding enc) {
    if (str) {
        if (enc == NONE) enc = UTF8;
        return create(str, enc)->length();
    } else {
        return 0;
    }
}

inline Buffer::Ptr Buffer::concat(JsArray::CPtr list, Size total) {
    Size len = list->length();
    Size sum = 0;
    for (Size i = 0; i < len; i++) {
        CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) sum += b->length();
    }
    Size bufLen = sum < total ? sum : total;

    Ptr buf = create(bufLen);
    Size offset = 0;
    for (Size i = 0; i < len; i++) {
        CPtr b = toCPtr<Buffer>(list->get(i));
        if (b) offset += b->copy(buf, offset);
    }
    return buf;
}

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_IMPL_BUFFER_H_
