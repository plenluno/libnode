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

class OnData : LIBJ_JS_FUNCTION(OnData)
 private:
    String::CPtr body_;

 public:
    OnData() : body_(String::create()) {}

    String::CPtr getBody() const { return body_; }

    Value operator()(JsArray::Ptr args) {
        String::CPtr chunk = toCPtr<String>(args->get(0));
        if (chunk)
            body_ = body_->concat(chunk);
        return Status::OK;
    }
};

class Send : LIBJ_JS_FUNCTION(Send)
 private:
    http::Server::Ptr srv_;
    http::ServerRequest::Ptr req_;
    http::ServerResponse::Ptr res_;
    String::CPtr body_;

 public:
    Send(
        http::Server::Ptr srv,
        http::ServerRequest::Ptr req,
        http::ServerResponse::Ptr res,
        String::CPtr body)
        : srv_(srv)
        , req_(req)
        , res_(res)
        , body_(body) {}

    Value operator()(JsArray::Ptr args) {
        res_->write(body_);
        res_->end();
        req_->removeAllListeners();
        // only one reply
        srv_->removeAllListeners();
        srv_->close();
        return Status::OK;
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

    Value operator()(JsArray::Ptr args) {
        req_->put(String::create("body"), onData_->getBody());
        String::CPtr body = json::stringify(req_);

        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));
        res_->setHeader(
            http::HEADER_CONTENT_LENGTH,
            String::valueOf(Buffer::byteLength(body)));
        Send::Ptr send(new Send(srv_, req_, res_, body));
#ifdef USE_SET_TIMEOUT
        // 3 sec delay
        setTimeout(send, 3000);
#else
        send->call();
#endif
        return Status::OK;
    }
};

class OnClose :  LIBJ_JS_FUNCTION(OnClose)
 public:
    Value operator()(JsArray::Ptr args) {
        console::log(String::create("event 'close'"));
        return Status::OK;
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
    Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));

        OnData::Ptr onData(new OnData());
        OnEnd::Ptr onEnd(new OnEnd(srv_, req, res, onData));
        OnClose::Ptr onClose(new OnClose());
        req->setEncoding(Buffer::UTF8);
        req->on(http::ServerRequest::EVENT_DATA, onData);
        req->on(http::ServerRequest::EVENT_END, onEnd);
        req->on(http::ServerRequest::EVENT_CLOSE, onClose);
        res->on(http::ServerResponse::EVENT_CLOSE, onClose);

        console::printv(console::NORMAL,
            "method: %v\n", req->method());
        console::printv(console::NORMAL,
            "url: %v\n", req->url());
        console::printv(console::NORMAL,
            "version: HTTP/%v\n", req->httpVersion());
        console::printv(console::NORMAL,
            "address: %v\n", json::stringify(req->connection()->address()));
        console::printv(console::NORMAL,
            "remote address: %v\n", req->connection()->remoteAddress());
        return Status::OK;
    }
};

}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;
    namespace http = libj::node::http;

    http::Server::Ptr server = http::Server::create();
    node::OnRequest::Ptr onRequest(new node::OnRequest(server));
    server->on(http::Server::EVENT_REQUEST, onRequest);
    server->listen(10000);
    node::run();
    return 0;
}
