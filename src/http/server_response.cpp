// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <list>

#include "./server_response_impl.h"

namespace libj {
namespace node {
namespace http {

#define LIBNODE_SERVER_RESPONSE_HEADER_DEF_GEN(NAME, VAL) \
    const String::CPtr ServerResponse::NAME = String::create(VAL);

#define LIBNODE_HTTP_SERVER_RESPONSE_HEADER_MAP(GEN) \
    GEN(HEADER_CACHE_CONTROL, "Cache-Control") \
    GEN(HEADER_CONNECTION, "Connection") \
    GEN(HEADER_DATE, "Date") \
    GEN(HEADER_PRAGMA, "Pragma") \
    GEN(HEADER_TRAILER, "Trailer") \
    GEN(HEADER_TRANSFER_ENCODING, "Transfer-Encoding") \
    GEN(HEADER_UPGRADE, "Upgrade") \
    GEN(HEADER_VIA, "Via") \
    GEN(HEADER_WARNING, "Warning") \
    GEN(HEADER_ALLOW, "Allow") \
    GEN(HEADER_CONTENT_ENCODING, "Content-Encoding") \
    GEN(HEADER_CONTENT_LANGUAGE, "Content-Language") \
    GEN(HEADER_CONTENT_LENGTH, "Content-Length") \
    GEN(HEADER_CONTENT_LOCATION, "Content-Location") \
    GEN(HEADER_CONTENT_MD5, "Content-MD5") \
    GEN(HEADER_CONTENT_RANGE, "Content-Range") \
    GEN(HEADER_CONTENT_TYPE, "Content-Type") \
    GEN(HEADER_EXPIRES, "Expires") \
    GEN(HEADER_LAST_MODIFIED, "Last-Modified") \
    GEN(HEADER_ACCEPT_RANGES, "Accept-Ranges") \
    GEN(HEADER_AGE, "Age") \
    GEN(HEADER_ETAG, "ETag") \
    GEN(HEADER_LOCATION, "Location") \
    GEN(HEADER_PROXY_AUTHENTICAE, "Proxy-Authenticate") \
    GEN(HEADER_RETRY_AFTER, "Retry-After") \
    GEN(HEADER_SERVER, "Server") \
    GEN(HEADER_VARY, "Vary") \
    GEN(HEADER_WWW_AUTHENTICATE, "WWW-Authenticate") \
    GEN(HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, \
        "Access-Control-Allow-Origin") \
    GEN(HEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS, \
        "Access-Control-Allow-Credentials") \
    GEN(HEADER_ACCESS_CONTROL_EXPOSE_HEADERS, \
        "Access-Control-Expose-Headers") \
    GEN(HEADER_ACCESS_CONTROL_MAX_AGE, \
        "Access-Control-Max-Age") \
    GEN(HEADER_ACCESS_CONTROL_ALLOW_METHODS, \
        "Access-Control-Allow-Methods") \
    GEN(HEADER_ACCESS_CONTROL_ALLOW_HEADERS, \
        "Access-Control-Allow-Headers")

LIBNODE_HTTP_SERVER_RESPONSE_HEADER_MAP(
    LIBNODE_SERVER_RESPONSE_HEADER_DEF_GEN)

const String::CPtr ServerResponseImpl::HEADERS =
    String::create("headers");
const String::CPtr ServerResponseImpl::STATUS_CODE =
    String::create("statusCode");

}  // namespace http
}  // namespace node
}  // namespace libj
