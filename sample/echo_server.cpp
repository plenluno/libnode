#include "libj/json.h"
#include "libj/console.h"
#include "libnode/node.h"
#include "libnode/http_server.h"
#include "libnode/http_server_request.h"
#include "libnode/http_server_response.h"
#include <iostream>

namespace libj {
namespace node {

class OnData : LIBJ_JS_FUNCTION(OnData)
 private:
    String::CPtr body_;
    
 public:
    OnData() : body_(String::create()) {}
    
    String::CPtr getBody() const { return body_; }
    
    Value operator()(JsArray::CPtr args) {
        String::CPtr chunk = toCPtr<String>(args->get(0));
        if (chunk)
            body_ = body_->concat(chunk);
        return 0;
    }
};

class OnEnd : LIBJ_JS_FUNCTION(OnEnd)
 private:
    http::Server::Ptr srv_;
    http::ServerRequest::Ptr req_;
    http::ServerResponse::Ptr res_;
    OnData::Ptr onData_;
    
 public:
    OnEnd(
        http::Server::Ptr srv,
        http::ServerRequest::Ptr req,
        http::ServerResponse::Ptr res,
        OnData::Ptr onData)
        : srv_(srv)
        , req_(req)
        , res_(res)
        , onData_(onData) {}
    
    Value operator()(JsArray::CPtr args) {
        req_->put(String::create("body"), onData_->getBody());
        res_->setHeader(String::create("Content-Type"), String::create("text/plain"));
        res_->write(json::stringify(req_));
        res_->end();
        req_->removeAllListeners(http::ServerRequest::EVENT_DATA);
        req_->removeAllListeners(http::ServerRequest::EVENT_END);
        // only one reply
        srv_->close();
        return 0;
    }
};

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
 private:
    http::Server::Ptr srv_;

 public:
    OnRequest(
        http::Server::Ptr srv)
        : srv_(srv) {}

 public:
    Value operator()(JsArray::CPtr args) {
        http::ServerRequest::Ptr req = toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res = toPtr<http::ServerResponse>(args->get(1));
        OnData::Ptr onData(new OnData());
        OnEnd::Ptr onEnd(new OnEnd(srv_, req, res, onData));
        req->on(http::ServerRequest::EVENT_DATA, onData);
        req->on(http::ServerRequest::EVENT_END, onEnd);
        console::log(String::create("method: ")->concat(
            req->method()));
        console::log(String::create("version: HTTP/")->concat(
            req->httpVersion()));
        console::log(String::create("remote address: ")->concat(
            req->connection()->remoteAddress()));
        return 0;
    }
};

}  // node
}  // libj

int main() {
    using namespace libj;
    using namespace node;
    http::Server::Ptr server = http::Server::create();
    OnRequest::Ptr onRequest(new OnRequest(server));
    server->on(http::Server::EVENT_REQUEST, onRequest);
    server->listen(10000);
    run();
    return 0;
}

