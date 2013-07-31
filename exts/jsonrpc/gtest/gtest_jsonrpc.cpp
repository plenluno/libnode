// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/node.h>
#include <libnode/jsonrpc.h>

#include <libj/json.h>
#include <libj/console.h>

namespace libj {
namespace node {
namespace jsonrpc {

class Add : LIBJ_JS_FUNCTION(Add)
 public:
    virtual Value operator()(JsArray::Ptr args) {
        Response::Ptr r = args->getPtr<Response>(2);
        Long x, y;
        if (to<Long>(args->get(0), &x) &&
            to<Long>(args->get(1), &y)) {
            return r->result(x + y);
        } else {
            return r->error(Error::create(Error::INVALID_PARAMS));
        }
    }
};

class Close : LIBJ_JS_FUNCTION(Close)
 public:
    Close(Service::Ptr service)
        : done_(false)
        , service_(service) {}

    virtual Value operator()(JsArray::Ptr args) {
        Response::Ptr r = args->getPtr<Response>(0);
        service_->close();
        done_ = true;
        return r->result(UNDEFINED);
    }

    Boolean done() {
        return done_;
    }

 private:
    Boolean done_;
    Service::Ptr service_;
};

class OnResponse : LIBJ_JS_FUNCTION(OnResponse)
 public:
    OnResponse() : responses_(JsArray::create()) {}

    virtual Value operator()(JsArray::Ptr args) {
        responses_->add(args->get(0));
        return Status::OK;
    }

    JsArray::CPtr responses() {
        return responses_;
    }

 private:
    JsArray::Ptr responses_;
};


static Int getCode(JsObject::CPtr res) {
    JsObject::CPtr e = res->getCPtr<JsObject>(ERROR);
    if (e) {
        return libj::to<Int>(e->get(CODE), 0);
    } else {
        return 0;
    }
}

static Int getId(JsObject::CPtr res) {
    return libj::to<Long>(res->get(ID), -1);
}

TEST(GTestJsonRpc, TestJsonRpc10) {
    Service::Ptr service = Service::create();

    OnResponse::Ptr onResponse(new OnResponse());
    service->on(Service::EVENT_RESPONSE, onResponse);

    Add::Ptr add(new Add());
    Close::Ptr close(new Close(service));

    Method::Ptr methodAdd = Method::create(
        String::create("add"),
        toPtr<JsArray>(json::parse(String::create("[\"x\",\"y\"]"))),
        add);

    Method::Ptr methodClose = Method::create(
        String::create("close"),
        JsArray::create(),
        close);

    service->addMethod(methodAdd);
    service->addMethod(methodClose);

    service->postRequest(String::null());
    service->postRequest(String::create("abc"));
    service->postRequest(String::create("\"abc\""));
    service->postRequest(String::create("{}"));
    service->postRequest(String::create("{\"id\":1}"));
    service->postRequest(String::create(
        "{\"method\":\"none\",\"params\":[],\"id\":2}"));
    service->postRequest(String::create(
        "{\"method\":\"close\",\"params\":[0],\"id\":3}"));
    service->postRequest(String::create(
        "{\"method\":\"add\",\"params\":[3,5],\"id\":4}"));
    service->postRequest(String::create(
        "{\"method\":\"close\",\"params\":[],\"id\":null}"));

    node::run();

    JsArray::CPtr responses = onResponse->responses();

    ASSERT_EQ(-1, getId(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-1, getId(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-1, getId(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-1, getId(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(1,  getId(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(2,  getId(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(3,  getId(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(4,  getId(responses->getCPtr<JsObject>(7)));

    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(-32601, getCode(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(7)));

    ASSERT_TRUE(close->done());
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj
