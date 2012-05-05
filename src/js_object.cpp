// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/js_object.h"

namespace libj {
namespace node {

class JsObjectImpl : public JsObject {
 public:
    static Ptr create() {
        Ptr p(new JsObjectImpl());
        return p;
    }
    
    Type<String>::Cptr toString() const {
        return map_->toString();
    }

 private:
    Type<Map>::Ptr map_;

    JsObjectImpl() : map_(Map::create()) {
    }

    LIBJ_MAP_IMPL(map_);
};

Type<JsObject>::Ptr JsObject::create() {
    return JsObjectImpl::create();
}

}  // namespace node
}  // namespace libj
