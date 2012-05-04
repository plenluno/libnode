// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/url.h"

namespace libj {
namespace node {
namespace url {

Type<JsObject>::Ptr parse(Type<String>::Cptr urlStr) {
    // TODO: implement
    return JsObject::create();
}

Type<String>::Cptr format(Type<JsObject>::Cptr urlObj) {
    // TODO: implement
    return String::create();
}

}  // namespace url
}  // namespace node
}  // namespace libj
