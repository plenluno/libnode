// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/fs.h>
#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/url.h>

static libj::String::CPtr root;

namespace libj {
namespace node {

class OnRead : LIBJ_JS_FUNCTION(OnRead)
 private:
    http::ServerResponse::Ptr res_;

 public:
    OnRead(http::ServerResponse::Ptr res) : res_(res) {}

    Value operator()(JsArray::Ptr args) {
        Buffer::CPtr content = toCPtr<Buffer>(args->get(1));
        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));
        if (content) {
            res_->setHeader(
                http::HEADER_CONTENT_LENGTH,
                String::valueOf(content->length()));
            res_->write(content);
        } else {
            http::Status::CPtr status404 =
                http::Status::create(http::Status::NOT_FOUND);
            res_->writeHead(status404->code());
            res_->write(status404->message());
        }
        res_->end();
        return 0;
    }
};

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
 public:
    Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));
        JsObject::Ptr url = url::parse(req->url());
        OnRead::Ptr onRead(new OnRead(res));
        fs::readFile(
            root->concat(toCPtr<String>(url->get(url::PATHNAME))),
            onRead);
        return 0;
    }
};

}  // namespace node
}  // namespace libj

int main(int argc, char *argv[]) {
    namespace node = libj::node;
    namespace http = libj::node::http;

    if (argc < 2) {
        char dir[256];
        getcwd(dir, 256);
        root = libj::String::create(dir);
    } else {
        root = libj::String::create(argv[1]);
    }

    node::OnRequest::Ptr requestHandler(new node::OnRequest());
    http::Server::Ptr server = http::createServer(requestHandler);
    server->listen(10001);
    node::run();
    return 0;
}
