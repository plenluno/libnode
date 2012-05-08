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
    Type<String>::Cptr body_;
    
 public:
    OnData() : body_(String::create()) {}
    
    Type<String>::Cptr getBody() const { return body_; }
    
    Value operator()(Type<JsArray>::Cptr args) {
        Type<String>::Cptr chunk = toCptr<String>(args->get(0));
        if (chunk)
            body_ = body_->concat(chunk);
        return 0;
    }
};

class OnEnd : LIBNODE_JS_FUNCTION(OnEnd)
 private:
    Type<OnData>::Ptr onData_;
    Type<http::ServerRequest>::Ptr req_;
    Type<http::ServerResponse>::Ptr res_;
    
 public:
    OnEnd(
        Type<OnData>::Ptr onData,
        Type<http::ServerRequest>::Ptr req,
        Type<http::ServerResponse>::Ptr res)
        : onData_(onData)
        , req_(req)
        , res_(res) {}
    
    Value operator()(Type<JsArray>::Cptr args) {
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
    Value operator()(Type<JsArray>::Cptr args) {
        Type<http::ServerRequest>::Ptr req = toPtr<http::ServerRequest>(args->get(0));
        Type<http::ServerResponse>::Ptr res = toPtr<http::ServerResponse>(args->get(1));
        Type<OnData>::Ptr onData(new OnData());
        Type<OnEnd>::Ptr onEnd(new OnEnd(onData, req, res));
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
    Type<OnRequest>::Ptr requestHandler(new OnRequest());
    Type<http::Server>::Ptr server = http::Server::create(requestHandler);
    server->listen(10000);
    run();
    return 0;
}

