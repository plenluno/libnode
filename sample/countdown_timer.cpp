// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/console.h>
#include <libj/string.h>

#include "libnode/node.h"
#include "libnode/timer.h"

namespace libj {
namespace node {

Int count = 100;

class OnInterval : LIBJ_JS_FUNCTION(OnInterval)
 private:
    Value id_;

 public:
    void setId(Value id) {
        id_ = id;
    }

    Value operator()(JsArray::Ptr args) {
        if (count) {
            console::log(String::valueOf(count--));
        } else {
            clearInterval(id_);
        }
        return 0;
    }
};

}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;

    node::OnInterval::Ptr onInterval(new node::OnInterval);
    libj::JsArray::Ptr args = libj::JsArray::create();
    libj::Value id = node::setInterval(onInterval, 100, args);
    onInterval->setId(id);

    node::run();
    return 0;
}
