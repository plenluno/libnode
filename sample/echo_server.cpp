#include "libnode/node.h"
#include "libnode/http_server.h"

int main() {
    using namespace libj;
    using namespace libj::node;
    Type<http::Server>::Ptr server = http::Server::create();
    server->listen(10000);
    run();
    return 0;
}
