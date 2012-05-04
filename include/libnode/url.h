// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_URL_H_
#define LIBNODE_URL_H_

#include "libj/string.h"
#include "libnode/typedef.h"

namespace libj {
namespace node {

class Url : LIBNODE_MODULE(Url)
 public:
    Type<JsObject>::Ptr parse(Type<String>::Cptr urlStr);
    Type<String>::Cptr format(Type<JsObject>::Cptr urlObj);
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_