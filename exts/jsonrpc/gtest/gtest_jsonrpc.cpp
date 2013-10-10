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
        Respond::Ptr r = args->getPtr<Respond>(2);
        Long x, y;
        if (to<Long>(args->get(0), &x) &&
            to<Long>(args->get(1), &y)) {
            return r->result(x + y);
        } else {
            return r->error(Error::create(Error::INVALID_PARAMS));
        }
    }
};

class Stop : LIBJ_JS_FUNCTION(Stop)
 public:
    Stop(Service::Ptr service)
        : done_(false)
        , service_(service) {}

    virtual Value operator()(JsArray::Ptr args) {
        Respond::Ptr r = args->getPtr<Respond>(0);
        done_ = service_->stop();
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
        return libj::to<Int>(e->get(CODE), -1);
    } else {
        return 0;
    }
}

static Long getId(JsObject::CPtr res) {
    Value id = res->get(ID);
    if (id.isNull()) {
        return 0;
    } else {
        return libj::to<Long>(id, -1);
    }
}

static Boolean isJsonRpc20(JsObject::CPtr res) {
    if (res) {
        String::CPtr s = res->getCPtr<String>(JSONRPC);
        return s && s->equals(String::create("2.0"));
    } else {
        return false;
    }
}

TEST(GTestJsonRpc, TestMemoryLeak) {
    Service::Ptr service = Service::create();
    ASSERT_TRUE(!!service);
}

TEST(GTestJsonRpc, TestJsonRpc10) {
    Service::Ptr service = Service::create();
    ASSERT_TRUE(service->start());
    ASSERT_TRUE(service->isRunning());

    OnResponse::Ptr onResponse(new OnResponse());
    service->on(Service::EVENT_RESPONSE, onResponse);

    Add::Ptr add(new Add());
    Stop::Ptr stop(new Stop(service));

    Method::Ptr methodAdd = Method::create(
        String::create("add"),
        toPtr<JsArray>(json::parse(String::create("[\"x\",\"y\"]"))),
        add);

    Method::Ptr methodStop = Method::create(
        String::create("stop"),
        JsArray::create(),
        stop);

    service->addMethod(methodAdd);
    service->addMethod(methodStop);

    service->postRequest(String::create("{}"));
    service->postRequest(String::create("{\"id\":1}"));
    service->postRequest(String::create(
        "{\"method\":\"none\",\"params\":[],\"id\":2}"));
    service->postRequest(String::create(
        "{\"method\":\"stop\",\"params\":[0],\"id\":3}"));
    service->postRequest(String::create(
        "{\"method\":\"add\",\"params\":[3,5],\"id\":4}"));
    service->postRequest(String::create(
        "{\"method\":\"stop\",\"params\":[],\"id\":null}"));

    node::run();

    JsArray::CPtr responses = onResponse->responses();

    Size len = responses->length();
    ASSERT_EQ(5, len);

    for (Size i = 0; i < len; i++) {
        ASSERT_FALSE(isJsonRpc20(responses->getCPtr<JsObject>(i)));
    }

    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(1, getId(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(2, getId(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(3, getId(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(4, getId(responses->getCPtr<JsObject>(4)));

    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-32601, getCode(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(4)));

    ASSERT_TRUE(responses->getCPtr<JsObject>(4)
        ->get(RESULT).equals(static_cast<Long>(8)));

    ASSERT_TRUE(stop->done());
    ASSERT_FALSE(service->isRunning());
}

TEST(GTestJsonRpc, Test2JsonRpc10) {
    Service::Ptr service = Service::create();
    ASSERT_TRUE(service->start());
    ASSERT_TRUE(service->isRunning());

    OnResponse::Ptr onResponse(new OnResponse());

    Add::Ptr add(new Add());
    Stop::Ptr stop(new Stop(service));

    Method::Ptr methodAdd = Method::create(
        String::create("add"),
        toPtr<JsArray>(json::parse(String::create("[\"x\",\"y\"]"))),
        add);

    Method::Ptr methodStop = Method::create(
        String::create("stop"),
        JsArray::create(),
        stop);

    service->addMethod(methodAdd);
    service->addMethod(methodStop);

    service->postRequest(String::create("{}"), onResponse);
    service->postRequest(String::create("{\"id\":1}"), onResponse);
    service->postRequest(
        String::create("{\"method\":\"none\",\"params\":[],\"id\":2}"),
        onResponse);
    service->postRequest(
        String::create("{\"method\":\"stop\",\"params\":[0],\"id\":3}"),
        onResponse);
    service->postRequest(
        String::create("{\"method\":\"add\",\"params\":[3,5],\"id\":4}"),
        onResponse);
    service->postRequest(
        String::create("{\"method\":\"stop\",\"params\":[],\"id\":null}"),
        onResponse);

    node::run();

    JsArray::CPtr responses = onResponse->responses();

    Size len = responses->length();
    ASSERT_EQ(6, len);

    for (Size i = 0; i < len; i++) {
        ASSERT_FALSE(isJsonRpc20(responses->getCPtr<JsObject>(i)));
    }

    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(1, getId(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(2, getId(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(3, getId(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(4, getId(responses->getCPtr<JsObject>(4)));

    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-32601, getCode(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(4)));

    ASSERT_TRUE(responses->getCPtr<JsObject>(4)
        ->get(RESULT).equals(static_cast<Long>(8)));

    ASSERT_TRUE(!responses->getCPtr<JsObject>(5));

    ASSERT_TRUE(stop->done());
    ASSERT_FALSE(service->isRunning());
}

TEST(GTestJsonRpc, TestJsonRpc20) {
    Service::Ptr service = Service::create();
    ASSERT_TRUE(service->start());
    ASSERT_TRUE(service->isRunning());

    OnResponse::Ptr onResponse(new OnResponse());
    service->on(Service::EVENT_RESPONSE, onResponse);

    Add::Ptr add(new Add());
    Stop::Ptr stop(new Stop(service));

    Method::Ptr methodAdd = Method::create(
        String::create("add"),
        toPtr<JsArray>(json::parse(String::create("[\"x\",\"y\"]"))),
        add);

    Method::Ptr methodStop = Method::create(
        String::create("stop"),
        JsArray::create(),
        stop);

    service->addMethod(methodAdd);
    service->addMethod(methodStop);

    service->postRequest(String::null());
    service->postRequest(String::create("abc"));
    service->postRequest(String::create("\"abc\""));
    service->postRequest(String::create("{\"jsonrpc\":\"2.0\"}"));
    service->postRequest(String::create("{\"jsonrpc\":\"2.0\",\"id\":1}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"a\",\"params\":[],\"id\":[]}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"b\",\"params\":22,\"id\":2}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":33,\"params\":[],\"id\":3}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"none\",\"params\":[],\"id\":4}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"stop\",\"params\":[0],\"id\":5}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":{},\"id\":6}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":[3,5],\"id\":7}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\","
        "\"params\":{\"y\":6, \"x\":3},\"id\":8}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\","
        "\"params\":{\"y\":6, \"z\":3},\"id\":9}"));
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"stop\"}"));

    node::run();

    JsArray::CPtr responses = onResponse->responses();

    Size len = responses->length();
    ASSERT_EQ(14, len);

    for (Size i = 0; i < len; i++) {
        ASSERT_TRUE(isJsonRpc20(responses->getCPtr<JsObject>(i)));
    }

    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(1, getId(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(2, getId(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(3, getId(responses->getCPtr<JsObject>(7)));
    ASSERT_EQ(4, getId(responses->getCPtr<JsObject>(8)));
    ASSERT_EQ(5, getId(responses->getCPtr<JsObject>(9)));
    ASSERT_EQ(6, getId(responses->getCPtr<JsObject>(10)));
    ASSERT_EQ(7, getId(responses->getCPtr<JsObject>(11)));
    ASSERT_EQ(8, getId(responses->getCPtr<JsObject>(12)));
    ASSERT_EQ(9, getId(responses->getCPtr<JsObject>(13)));

    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(7)));
    ASSERT_EQ(-32601, getCode(responses->getCPtr<JsObject>(8)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(9)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(10)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(11)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(12)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(13)));

    ASSERT_TRUE(responses->getCPtr<JsObject>(11)
        ->get(RESULT).equals(static_cast<Long>(8)));
    ASSERT_TRUE(responses->getCPtr<JsObject>(12)
        ->get(RESULT).equals(static_cast<Long>(9)));

    ASSERT_TRUE(stop->done());
    ASSERT_FALSE(service->isRunning());
}

TEST(GTestJsonRpc, Test2JsonRpc20) {
    Service::Ptr service = Service::create();
    ASSERT_TRUE(service->start());
    ASSERT_TRUE(service->isRunning());

    OnResponse::Ptr onResponse(new OnResponse());

    Add::Ptr add(new Add());
    Stop::Ptr stop(new Stop(service));

    Method::Ptr methodAdd = Method::create(
        String::create("add"),
        toPtr<JsArray>(json::parse(String::create("[\"x\",\"y\"]"))),
        add);

    Method::Ptr methodStop = Method::create(
        String::create("stop"),
        JsArray::create(),
        stop);

    service->addMethod(methodAdd);
    service->addMethod(methodStop);

    service->postRequest(String::null(), onResponse);
    service->postRequest(String::create("abc"), onResponse);
    service->postRequest(String::create("\"abc\""), onResponse);
    service->postRequest(String::create("{\"jsonrpc\":\"2.0\"}"), onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"id\":1}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"a\",\"params\":[],\"id\":[]}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"b\",\"params\":22,\"id\":2}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":33,\"params\":[],\"id\":3}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"none\",\"params\":[],\"id\":4}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"stop\",\"params\":[0],\"id\":5}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":{},\"id\":6}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":[3,5],\"id\":7}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\","
        "\"params\":{\"y\":6, \"x\":3},\"id\":8}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"add\","
        "\"params\":{\"y\":6, \"z\":3},\"id\":9}"),
        onResponse);
    service->postRequest(String::create(
        "{\"jsonrpc\":\"2.0\",\"method\":\"stop\"}"),
        onResponse);

    node::run();

    JsArray::CPtr responses = onResponse->responses();

    Size len = responses->length();
    ASSERT_EQ(15, len);

    for (Size i = 0; i < len - 1; i++) {
        ASSERT_TRUE(isJsonRpc20(responses->getCPtr<JsObject>(i)));
    }

    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(1, getId(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(0, getId(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(2, getId(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(3, getId(responses->getCPtr<JsObject>(7)));
    ASSERT_EQ(4, getId(responses->getCPtr<JsObject>(8)));
    ASSERT_EQ(5, getId(responses->getCPtr<JsObject>(9)));
    ASSERT_EQ(6, getId(responses->getCPtr<JsObject>(10)));
    ASSERT_EQ(7, getId(responses->getCPtr<JsObject>(11)));
    ASSERT_EQ(8, getId(responses->getCPtr<JsObject>(12)));
    ASSERT_EQ(9, getId(responses->getCPtr<JsObject>(13)));

    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(0)));
    ASSERT_EQ(-32700, getCode(responses->getCPtr<JsObject>(1)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(2)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(3)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(4)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(5)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(6)));
    ASSERT_EQ(-32600, getCode(responses->getCPtr<JsObject>(7)));
    ASSERT_EQ(-32601, getCode(responses->getCPtr<JsObject>(8)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(9)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(10)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(11)));
    ASSERT_EQ(0,      getCode(responses->getCPtr<JsObject>(12)));
    ASSERT_EQ(-32602, getCode(responses->getCPtr<JsObject>(13)));

    ASSERT_TRUE(responses->getCPtr<JsObject>(11)
        ->get(RESULT).equals(static_cast<Long>(8)));
    ASSERT_TRUE(responses->getCPtr<JsObject>(12)
        ->get(RESULT).equals(static_cast<Long>(9)));

    ASSERT_TRUE(!responses->getCPtr<JsObject>(14));

    ASSERT_TRUE(stop->done());
    ASSERT_FALSE(service->isRunning());
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj
