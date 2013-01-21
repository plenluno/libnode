// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_IMPL_SERVER_RESPONSE_H_
#define LIBNODE_DETAIL_HTTP_IMPL_SERVER_RESPONSE_H_

namespace libj {
namespace node {
namespace detail {
namespace http {

inline OutgoingMessage::Ptr OutgoingMessage::createInServer(
    IncomingMessage::Ptr req) {
    LIBJ_STATIC_SYMBOL_DEF(symChunked, "chunked");

    OutgoingMessage::Ptr self(new OutgoingMessage());
    self->setFlag(OutgoingMessage::SERVER_SIDE);
    self->setFlag(OutgoingMessage::SEND_DATE);

    if (req->method()->equals(node::http::METHOD_HEAD)) {
        self->unsetFlag(OutgoingMessage::HAS_BODY);
    }

    if (req->httpVersionMajor() < 1 ||
        (req->httpVersionMajor() == 1 &&
         req->httpVersionMinor() == 0)) {
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
        String::CPtr te = req->getHeader(node::http::HEADER_TE);
        if (te && te->equals(symChunked)) {
            self->setFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
        } else {
            self->unsetFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
        }
    }
    return self;
}

inline ServerResponse::Ptr ServerResponse::create(OutgoingMessage::Ptr msg) {
    if (msg) {
        return Ptr(new ServerResponse(msg));
    } else {
        return null();
    }
}

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_IMPL_SERVER_RESPONSE_H_
