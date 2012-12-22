// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
#define LIBNODE_GTEST_GTEST_HTTP_COMMON_H_

#include <libnode/http.h>

#include <libj/js_function.h>
#include <libj/string_buffer.h>

namespace libj {
namespace node {

class GTestHttpOnData : LIBJ_JS_FUNCTION(GTestHttpOnData)
 public:
    GTestHttpOnData() : body_(StringBuffer::create()) {}

    String::CPtr body() const { return body_->toString(); }

    virtual Value operator()(JsArray::Ptr args) {
        String::CPtr chunk = toCPtr<String>(args->get(0));
        if (chunk) {
            body_->append(chunk);
        }
        return Status::OK;
    }

 private:
    StringBuffer::Ptr body_;
};

class GTestHttpOnClose :  LIBJ_JS_FUNCTION(GTestHttpOnClose)
 public:
    static UInt count() { return count_; }

    static void clear() { count_ = 0; }

    virtual Value operator()(JsArray::Ptr args) {
        count_++;
        return Status::OK;
    }

 private:
    static UInt count_;
};

class GTestHttpClientOnEnd : LIBJ_JS_FUNCTION(GTestHttpClientOnEnd)
 public:
    GTestHttpClientOnEnd(GTestHttpOnData::Ptr onData)
        : onData_(onData) {}

    static JsArray::CPtr messages() { return msgs_; }

    static void clear() { msgs_ = JsArray::create(); }

    virtual Value operator()(JsArray::Ptr args) {
        msgs_->add(onData_->body());
        return Status::OK;
    }

 private:
    static JsArray::Ptr msgs_;

    GTestHttpOnData::Ptr onData_;
};

class GTestHttpClientOnResponse : LIBJ_JS_FUNCTION(GTestHttpClientOnResponse)
 public:
    virtual Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res =
            toPtr<http::ClientResponse>(args->get(0));

        GTestHttpOnData::Ptr onData(new GTestHttpOnData());
        GTestHttpOnClose::Ptr onClose(new GTestHttpOnClose());
        GTestHttpClientOnEnd::Ptr onEnd(new GTestHttpClientOnEnd(onData));
        res->setEncoding(Buffer::UTF8);
        res->on(http::ClientResponse::EVENT_DATA, onData);
        res->on(http::ClientResponse::EVENT_END, onEnd);
        res->on(http::ClientResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
