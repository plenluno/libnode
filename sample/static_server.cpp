#include "libnode/node.h"
#include "libnode/url.h"
#include "libnode/file_system.h"
#include "libnode/http_server.h"
#include "libnode/http_server_request.h"
#include "libnode/http_server_response.h"
#include "libnode/http_status.h"
#include <iostream>

static libj::String::CPtr root;

namespace libj {
namespace node {

class OnRead : LIBJ_JS_FUNCTION(OnRead)
 private:
    http::ServerResponse::Ptr res_;
    
 public:
    OnRead(http::ServerResponse::Ptr res) : res_(res) {}
    
    Value operator()(JsArray::CPtr args) {
        String::CPtr content = toCPtr<String>(args->get(1));
        res_->setHeader(String::create("Content-Type"), String::create("text/plain"));
        if (content) {
            res_->write(content);
        } else {
            res_->writeHead(http::Status::NOT_FOUND);
        }
        res_->end();
        return 0;
    }
};

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
 public:
    Value operator()(JsArray::CPtr args) {
        http::ServerRequest::Ptr req = toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res = toPtr<http::ServerResponse>(args->get(1));
        JsObject::Ptr url = url::parse(req->url());
        OnRead::Ptr onRead(new OnRead(res));
        fs::readFile(
            root->concat(toCPtr<String>(url->get(url::PATHNAME))),
            onRead);
        return 0;
    }
};

}  // node
}  // libj

int main(int argc, char *argv[]) {
    using namespace libj;
    using namespace node;
    if (argc < 2) {
        char dir[256];
        getcwd(dir, 256);
        root = String::create(dir);
    } else {
        root = String::create(argv[1]);
    }
    OnRequest::Ptr requestHandler(new OnRequest());
    http::Server::Ptr server = http::Server::create(requestHandler);
    server->listen(10001);
    run();
    return 0;
}
