// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_H_

#include "libnode/stream/writable_stream.h"

namespace libj {
namespace node {
namespace http {

class ServerResponse : LIBNODE_WRITABLE_STREAM(ServerResponse)
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

    static const String::CPtr HEADER_ACCEPT_RANGES;
    static const String::CPtr HEADER_AGE;
    static const String::CPtr HEADER_ETAG;
    static const String::CPtr HEADER_LOCATION;
    static const String::CPtr HEADER_PROXY_AUTHENTICAE;
    static const String::CPtr HEADER_RETRY_AFTER;
    static const String::CPtr HEADER_SERVER;
    static const String::CPtr HEADER_VARY;
    static const String::CPtr HEADER_WWW_AUTHENTICATE;

    static const String::CPtr HEADER_ACCESS_CONTROL_ALLOW_ORIGIN;
    static const String::CPtr HEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS;
    static const String::CPtr HEADER_ACCESS_CONTROL_EXPOSE_HEADERS;
    static const String::CPtr HEADER_ACCESS_CONTROL_MAX_AGE;
    static const String::CPtr HEADER_ACCESS_CONTROL_ALLOW_METHODS;
    static const String::CPtr HEADER_ACCESS_CONTROL_ALLOW_HEADERS;

    virtual Boolean writeHead(Int statusCode) = 0;
    virtual Boolean writeHead(Int statusCode, String::CPtr reasonPhrase) = 0;
    virtual Int statusCode() const = 0;
    virtual void setHeader(String::CPtr name, String::CPtr value) = 0;
    virtual String::CPtr getHeader(String::CPtr name) const = 0;
    virtual void removeHeader(String::CPtr name) = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_H_
