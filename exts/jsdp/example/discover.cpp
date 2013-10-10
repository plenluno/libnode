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
        console::log(json::stringify(args->get(0)));
        return UNDEFINED;
    }
};

void discover() {
    DiscoveryService::Ptr ds = DiscoveryService::create(
        str("239.1.2.3"),
        12345);
    ds->join(Service::create(
        str("test1"),
        11111));
    ds->join(Service::create(
        str("test2"),
        22222,
        str("2.2.2"),
        str("password")));
    ds->start();

    JsFunction::Ptr onResult(new OnResult());
    ds->discover(onResult);
    ds->discover(str("test1"), onResult);
    ds->discover(str("test2"), str("passwd"), onResult);
    ds->discover(str("test2"), str("password"), onResult);

    node::run();
}

}  // namespace jsdp
}  // namespace node
}  // namespace libj

int main() {
    libj::node::jsdp::discover();
    return 0;
}
