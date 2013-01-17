// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
#define LIBNODE_GTEST_GTEST_HTTP_COMMON_H_

#include <libnode/http.h>

#include "./gtest_common.h"

namespace libj {
namespace node {

class GTestHttpClientOnResponse : LIBJ_JS_FUNCTION(GTestHttpClientOnResponse)
 public:
    static JsArray::CPtr statusCodes() { return statusCodes_; }

    static void clear() { statusCodes_->clear(); }

    virtual Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res =
            toPtr<http::ClientResponse>(args->get(0));

        statusCodes_->add(res->statusCode());

        GTestOnData::Ptr onData(new GTestOnData());
        GTestOnClose::Ptr onClose(new GTestOnClose());
        GTestOnEnd::Ptr onEnd(new GTestOnEnd(onData));
        res->setEncoding(Buffer::UTF8);
        res->on(http::ClientResponse::EVENT_DATA, onData);
        res->on(http::ClientResponse::EVENT_END, onEnd);
        res->on(http::ClientResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }

 private:
    static JsArray::Ptr statusCodes_;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
