// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/console.h>
#include <libj/string.h>
#include <libnode/node.h>
#include <libnode/timer.h>

namespace libj {
namespace node {

class OnInterval : LIBJ_JS_FUNCTION(OnInterval)
 private:
    Value id_;
    Int count_;

 public:
    OnInterval() : count_(100) {}

    void setId(Value id) {
        id_ = id;
    }

    void setCount(Int cnt) {
        count_ = cnt;
    }

    Value operator()(JsArray::Ptr args) {
        if (count_) {
            console::log(String::valueOf(count_--));
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

    node::OnInterval::Ptr onInterval(new node::OnInterval);;
    libj::Value id = node::setInterval(
        onInterval, 200, libj::JsArray::null());
    onInterval->setId(id);
    onInterval->setCount(75);

    node::OnInterval::Ptr onInterval2(new node::OnInterval);
    libj::Value id2 = node::setInterval(
        onInterval2, 150, libj::JsArray::null());
    onInterval2->setId(id2);
    onInterval2->setCount(100);

    node::run();
    return 0;
}
