// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/url.h"

namespace libj {
namespace node {

Type<JsObject>::Ptr Url::parse(Type<String>::Cptr urlStr) {
    // TODO: implement
    return JsObject::create();
}

Type<String>::Cptr Url::format(Type<JsObject>::Cptr urlObj) {
    // TODO: implement
    return String::create();
}

}  // namespace node
}  // namespace libj
