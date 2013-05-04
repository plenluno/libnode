// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/fs.h>
#include <libnode/url.h>

#include "./gtest_http_common.h"

namespace libj {
namespace node {

class OnRead : LIBJ_JS_FUNCTION(OnRead)
 public:
    OnRead(http::ServerResponse::Ptr res) : res_(res) {}

    static void setServer(http::Server::Ptr srv) { srv_ = srv; }

    static void unsetServer() { srv_ = http::Server::null(); }

    virtual Value operator()(JsArray::Ptr args) {
        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));

        Buffer::CPtr content = toCPtr<Buffer>(args->get(1));
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
        if (srv_) srv_->close();
        return Status::OK;
    }

 private:
    static http::Server::Ptr srv_;

    http::ServerResponse::Ptr res_;
};

http::Server::Ptr OnRead::srv_ = http::Server::null();

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
 public:
    OnRequest(String::CPtr root) : root_(root) {}

    virtual Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));
        JsObject::Ptr url = url::parse(req->url());
        OnRead::Ptr onRead(new OnRead(res));
        String::CPtr path =
            root_->concat(toCPtr<String>(url->get(url::PATHNAME)));
        console::printv(console::LEVEL_INFO, "GET %v\n", path);
        fs::readFile(path, onRead);
        return Status::OK;
    }

 private:
    String::CPtr root_;
};

TEST(GTestHttpStatic, TestStatic) {
    GTestOnEnd::clear();
    GTestOnClose::clear();

    char dir[256];
    getcwd(dir, 256);
    String::CPtr root = String::create(dir);

    OnRequest::Ptr requestHandler(new OnRequest(root));
    http::Server::Ptr server = http::createServer(requestHandler);
    server->listen(10000);
    OnRead::setServer(server);

    String::CPtr url = String::create("http://127.0.0.1:10000/CMakeCache.txt");
    JsObject::Ptr options = url::parse(url);
    JsObject::Ptr headers = JsObject::create();
    headers->put(
        http::HEADER_CONNECTION,
        String::create("close"));
    options->put(http::OPTION_HEADERS, headers);

    JsFunction::Ptr onResponse(new GTestHttpClientOnResponse());
    http::get(options, onResponse);

    node::run();

    ASSERT_EQ(0, GTestOnClose::count());

    JsArray::CPtr messages = GTestOnEnd::messages();
    ASSERT_EQ(1, messages->length());
    ASSERT_TRUE(messages->getCPtr<String>(0)->length());

    OnRead::unsetServer();
}

}  // namespace node
}  // namespace libj
