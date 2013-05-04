// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_HEADER_H_
#define LIBNODE_HTTP_HEADER_H_

#include <libj/symbol.h>

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_HEADER_MAP(GEN) \
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
    GEN(HEADER_ACCEPT, "Accept") \
    GEN(HEADER_ACCEPT_CHARSET, "Accept-Charset") \
    GEN(HEADER_ACCEPT_ENCODING, "Accept-Encoding") \
    GEN(HEADER_ACCEPT_LANGUAGE, "Accept-Language") \
    GEN(HEADER_AUTHORIZATION, "Authorization") \
    GEN(HEADER_EXPECT, "Expect") \
    GEN(HEADER_FROM, "From") \
    GEN(HEADER_HOST, "Host") \
    GEN(HEADER_IF_MODIFIED_SINCE, "If-Modified-Since") \
    GEN(HEADER_IF_MATCH, "If-Match") \
    GEN(HEADER_IF_NONE_MATCH, "If-None-Match") \
    GEN(HEADER_IF_RANGE, "If-Range") \
    GEN(HEADER_IF_UNMODIFIED_SINCE, "If-Unmodified-Since") \
    GEN(HEADER_MAX_FORWARDS, "Max-Forwards") \
    GEN(HEADER_PROXY_AUTHORIZATION, "Proxy-Authorization") \
    GEN(HEADER_RANGE, "Range") \
    GEN(HEADER_REFERER, "Referer") \
    GEN(HEADER_TE, "TE") \
    GEN(HEADER_USER_AGENT, "User-Agent") \
    GEN(HEADER_ACCEPT_RANGES, "Accept-Ranges") \
    GEN(HEADER_AGE, "Age") \
    GEN(HEADER_ETAG, "ETag") \
    GEN(HEADER_LOCATION, "Location") \
    GEN(HEADER_PROXY_AUTHENTICATE, "Proxy-Authenticate") \
    GEN(HEADER_RETRY_AFTER, "Retry-After") \
    GEN(HEADER_SERVER, "Server") \
    GEN(HEADER_VARY, "Vary") \
    GEN(HEADER_WWW_AUTHENTICATE, "WWW-Authenticate") \
    GEN(HEADER_LINK, "Link") \
    GEN(HEADER_COOKIE, "Cookie") \
    GEN(HEADER_SET_COOKIE, "Set-Cookie") \
    GEN(HEADER_ORIGIN, "Origin") \
    GEN(HEADER_ACCESS_CONTROL_REQUEST_METHOD, \
        "Access-Control-Request-Method") \
    GEN(HEADER_ACCESS_CONTROL_REQUEST_HEADERS, \
        "Access-Control-Request-Headers") \
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
        "Access-Control-Allow-Headers") \
    GEN(HEADER_SEC_WEBSOCKET_KEY, "Sec-WebSocket-Key") \
    GEN(HEADER_SEC_WEBSOCKET_EXTENSIONS, "Sec-WebSocket-Extensions") \
    GEN(HEADER_SEC_WEBSOCKET_ACCEPT, "Sec-WebSocket-Accept") \
    GEN(HEADER_SEC_WEBSOCKET_PROTOCOL, "Sec-WebSocket-Protocol") \
    GEN(HEADER_SEC_WEBSOCKET_VERSION, "Sec-WebSocket-Version")

#define LIBNODE_HTTP_HEADER_DECL_GEN(NAME, VAL) \
    extern Symbol::CPtr NAME;

#define LIBNODE_HTTP_LHEADER_DECL_GEN(NAME, VAL) \
    extern Symbol::CPtr L##NAME;

LIBNODE_HTTP_HEADER_MAP(LIBNODE_HTTP_HEADER_DECL_GEN)

LIBNODE_HTTP_HEADER_MAP(LIBNODE_HTTP_LHEADER_DECL_GEN)

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_HEADER_H_
