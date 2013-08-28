// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/trace.h>

#include <libj/console.h>

#include <gflags/gflags.h>

DEFINE_int32(repeat, 1, "the number of repetitions");

namespace libj {
namespace node {
namespace example {

inline void printRequest(Int id, String::CPtr method, String::CPtr url) {
    console::printf(console::LEVEL_NORMAL, "[%d] ", id);
    console::printv(console::LEVEL_NORMAL, "%v %v\n", method, url);
}

class OnData : LIBJ_JS_FUNCTION(OnData)
 public:
    OnData(
        Int id,
        String::CPtr method,
        String::CPtr url)
        : id_(id)
        , method_(method)
        , url_(url) {}

    virtual Value operator()(JsArray::Ptr args) {
        printRequest(id_, method_, url_);
        console::printv(console::LEVEL_NORMAL, "%v\n\n", args->get(0));
        return UNDEFINED;
    }

 private:
    Int id_;
    String::CPtr method_;
    String::CPtr url_;
};

class OnResponse : LIBJ_JS_FUNCTION(OnResponse)
 public:
    OnResponse(
        Int id,
        String::CPtr method,
        String::CPtr url)
        : id_(id)
        , method_(method)
        , url_(url) {}

    virtual Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res = args->getPtr<http::ClientResponse>(0);
        res->on(
            http::ClientResponse::EVENT_DATA,
            JsFunction::Ptr(new OnData(id_, method_, url_)));
        return UNDEFINED;
    }

 private:
    Int id_;
    String::CPtr method_;
    String::CPtr url_;
};

class OnError : LIBJ_JS_FUNCTION(OnError)
 public:
    OnError(
        Int id,
        String::CPtr method,
        String::CPtr url)
        : id_(id)
        , method_(method)
        , url_(url) {}

    virtual Value operator()(JsArray::Ptr args) {
        printRequest(id_, method_, url_);
        console::printv(console::LEVEL_NORMAL, "%v\n\n", args->get(0));
        return UNDEFINED;
    }

 private:
    Int id_;
    String::CPtr method_;
    String::CPtr url_;
};

inline void httpClient(String::CPtr url) {
    LIBJ_STATIC_SYMBOL_DEF(symGet, "GET");

    http::ClientRequest::Ptr req;
    for (Int i = 0; i < FLAGS_repeat; i++) {
        req = http::request(
            url,
            JsFunction::Ptr(new OnResponse(i, symGet, url)));
        req->on(
            http::ClientRequest::EVENT_ERROR,
            JsFunction::Ptr(new OnError(i, symGet, url)));
        req->end();
    }
    node::run();
}

}  // namespace exampel
}  // namespace node
}  // namespace libj

int main(int argc, char** argv) {
    std::string usage = "usage: http-client url";
    gflags::SetUsageMessage("\n\n" + usage);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (argc < 2) {
        libj::console::log(usage.data());
        return 0;
    }

    LIBNODE_DEBUG_TRACE_ON;
    namespace node = libj::node;
    node::example::httpClient(libj::String::create(argv[1]));
    LIBNODE_DEBUG_TRACE_OFF;
    return 0;
}
