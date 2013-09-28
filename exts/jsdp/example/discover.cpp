// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/jsdp.h>
#include <libnode/node.h>

#include <libj/console.h>
#include <libj/json.h>

namespace libj {
namespace node {
namespace jsdp {

class OnResult : LIBJ_JS_FUNCTION(OnResult)
 public:
    virtual Value operator()(JsArray::Ptr args) {
        console::log(json::stringify(args));
        return UNDEFINED;
    }
};

void discover() {
    DiscoveryService::Ptr ds = DiscoveryService::create(
        String::create("239.1.2.3"),
        12345);
    ds->discover(JsFunction::Ptr(new OnResult()));
    node::run();
}

}  // namespace jsdp
}  // namespace node
}  // namespace libj

int main() {
    libj::node::jsdp::discover();
    return 0;
}
