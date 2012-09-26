// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_REQUEST_H_
#define LIBNODE_HTTP_SERVER_REQUEST_H_

#include "libnode/stream/readable_stream.h"
#include "libnode/net/socket.h"

namespace libj {
namespace node {
namespace http {

class ServerRequest : LIBNODE_READABLE_STREAM(ServerRequest)
 public:
    static const String::CPtr HEADER_CACHE_CONTROL;
    static const String::CPtr HEADER_CONNECTION;
    static const String::CPtr HEADER_DATE;
    static const String::CPtr HEADER_PRAGMA;
    static const String::CPtr HEADER_TRAILER;
    static const String::CPtr HEADER_TRANSFER_ENCODING;
    static const String::CPtr HEADER_UPGRADE;
    static const String::CPtr HEADER_VIA;
    static const String::CPtr HEADER_WARNING;

    static const String::CPtr HEADER_ALLOW;
    static const String::CPtr HEADER_CONTENT_ENCODING;
    static const String::CPtr HEADER_CONTENT_LANGUAGE;
    static const String::CPtr HEADER_CONTENT_LENGTH;
    static const String::CPtr HEADER_CONTENT_LOCATION;
    static const String::CPtr HEADER_CONTENT_MD5;
    static const String::CPtr HEADER_CONTENT_RANGE;
    static const String::CPtr HEADER_CONTENT_TYPE;
    static const String::CPtr HEADER_EXPIRES;
    static const String::CPtr HEADER_LAST_MODIFIED;

    static const String::CPtr HEADER_ACCEPT;
    static const String::CPtr HEADER_ACCEPT_CHARSET;
    static const String::CPtr HEADER_ACCEPT_ENCODING;
    static const String::CPtr HEADER_ACCEPT_LANGUAGE;
    static const String::CPtr HEADER_AUTHORIZATION;
    static const String::CPtr HEADER_EXPECT;
    static const String::CPtr HEADER_FROM;
    static const String::CPtr HEADER_HOST;
    static const String::CPtr HEADER_IF_MODIFIED_SINCE;
    static const String::CPtr HEADER_IF_MATCH;
    static const String::CPtr HEADER_IF_NONE_MATCH;
    static const String::CPtr HEADER_IF_RANGE;
    static const String::CPtr HEADER_IF_UNMODIFIED_SINCE;
    static const String::CPtr HEADER_MAX_FORWARDS;
    static const String::CPtr HEADER_PROXY_AUTHORIZATION;
    static const String::CPtr HEADER_RANGE;
    static const String::CPtr HEADER_REFERER;
    static const String::CPtr HEADER_TE;
    static const String::CPtr HEADER_USER_AGENT;

    static const String::CPtr HEADER_ORIGIN;
    static const String::CPtr HEADER_ACCESS_CONTROL_REQUEST_METHOD;
    static const String::CPtr HEADER_ACCESS_CONTROL_REQUEST_HEADERS;

    virtual String::CPtr method() const = 0;
    virtual String::CPtr url() const = 0;
    virtual JsObject::CPtr headers() const = 0;
    virtual String::CPtr httpVersion() const = 0;
    virtual net::Socket::Ptr connection() const = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_REQUEST_H_
