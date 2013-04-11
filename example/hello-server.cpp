#include <libnode/http.h>
#include <libnode/node.h>
#include <libj/js_closure.h>

namespace libj {
namespace node {
namespace example {

void helloServer() {
    auto srv = http::Server::create(
        JsClosure::create([] (JsArray::Ptr args) {
            auto res = args->getPtr<http::ServerResponse>(1);
            res->setHeader(
                http::HEADER_CONTENT_TYPE,
                String::create("text/plain"));
            res->end(String::create("Hello World\n"));
            return UNDEFINED;
        }));
    srv->listen(1337, String::create("127.0.0.1"));
    node::run();
}

}  // namespace exampel
}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;
    node::example::helloServer();
    return 0;
}
