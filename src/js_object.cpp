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
    
    Value put(const Value& key, const Value& val) {
        return map_->put(String::valueOf(key), val);
    }
    
    Type<String>::Cptr toString() const {
        return map_->toString();
    }
    
    Size size() const {
        return map_->size();
    }
    
    Value get(const Value& key) const {
        return map_->get(key);
    }
    
    Value remove(const Value& key) {
        return map_->remove(key);
    }
    
    Type<Set>::Cptr keySet() const {
        return map_->keySet();
    }
    
    void clear() {
        map_->clear();
    }

 private:
    Type<Map>::Ptr map_;

    JsObjectImpl() : map_(Map::create()) {
    }
};

Type<JsObject>::Ptr JsObject::create() {
    return JsObjectImpl::create();
}

}  // namespace node
}  // namespace libj
