// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/console.h>
#include <libj/json.h>
#include <libj/status.h>
#include <libnode/buffer.h>
#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/timer.h>

namespace libj {
namespace node {
namespace echo {

}  // namespace echo
}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;
    namespace http = libj::node::http;

    http::ClientRequest::Ptr req = http::get(
        libj::String::create("http://127.0.0.1:10000/abc"),
        libj::JsFunction::null());
    node::run();
    return 0;
}
