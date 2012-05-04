#include "libnode/node.h"
#include "libnode/http_server.h"

int main() {
    using namespace libj;
    using namespace libj::node;
    Type<Node>::Ptr node = Node::instance();
    Type<HttpServer>::Ptr server = HttpServer::create();
    server->listen(10000);
    node->run();
    return 0;
}
