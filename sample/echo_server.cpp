#include "libj/json.h"
#include "libnode/node.h"
#include "libnode/http_server.h"
#include "libnode/http_server_request.h"
#include "libnode/http_server_response.h"

namespace libj {
namespace node {

class RequestHandler : LIBNODE_JS_FUNCTION(RequestHandler)
 public:
    Value operator()(Type<JsArray>::Cptr args) {
        Type<http::ServerRequest>::Ptr req = toPtr<http::ServerRequest>(args->get(0));
        Type<http::ServerResponse>::Ptr res = toPtr<http::ServerResponse>(args->get(1));
        res->setHeader(String::create("Content-Type"), String::create("text/plain"));
        res->write(json::stringify(req));
        res->end();
        return 0;
    }
};

}  // node
}  // libj

int main() {
    using namespace libj;
    using namespace node;
    Type<JsFunction>::Ptr requestHandler(new RequestHandler());
    Type<http::Server>::Ptr server = http::Server::create(requestHandler);
    server->listen(10000);
    run();
    return 0;
}


