#include "libj/json.h"
#include "libnode/node.h"
#include "libnode/http_server.h"
#include "libnode/http_server_request.h"
#include "libnode/http_server_response.h"
#include <iostream>

namespace libj {
namespace node {

class OnData : LIBNODE_JS_FUNCTION(OnData)
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

class OnEnd : LIBNODE_JS_FUNCTION(OnEnd)
 private:
    OnData::Ptr onData_;
    http::ServerRequest::Ptr req_;
    http::ServerResponse::Ptr res_;
    
 public:
    OnEnd(
        OnData::Ptr onData,
        http::ServerRequest::Ptr req,
        http::ServerResponse::Ptr res)
        : onData_(onData)
        , req_(req)
        , res_(res) {}
    
    Value operator()(JsArray::CPtr args) {
        req_->put(String::create("body"), onData_->getBody());
        res_->setHeader(String::create("Content-Type"), String::create("text/plain"));
        res_->write(json::stringify(req_));
        res_->end();
        req_->removeAllListeners(http::ServerRequest::EVENT_DATA);
        req_->removeAllListeners(http::ServerRequest::EVENT_END);
        return 0;
    }
};

class OnRequest : LIBNODE_JS_FUNCTION(OnRequest)
 public:
    Value operator()(JsArray::CPtr args) {
        http::ServerRequest::Ptr req = toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res = toPtr<http::ServerResponse>(args->get(1));
        OnData::Ptr onData(new OnData());
        OnEnd::Ptr onEnd(new OnEnd(onData, req, res));
        req->on(http::ServerRequest::EVENT_DATA, onData);
        req->on(http::ServerRequest::EVENT_END, onEnd);
        return 0;
    }
};

}  // node
}  // libj

int main() {
    using namespace libj;
    using namespace node;
    OnRequest::Ptr requestHandler(new OnRequest());
    http::Server::Ptr server = http::Server::create(requestHandler);
    server->listen(10000);
    run();
    return 0;
}

