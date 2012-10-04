// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>

#include "./server_request_impl.h"

namespace libj {
namespace node {
namespace http {

#define LIBNODE_SERVER_REQUEST_HEADER_DEF_GEN(NAME, VAL) \
    const String::CPtr ServerRequest::NAME = String::create(VAL);

#define LIBNODE_HTTP_SERVER_REQUEST_HEADER_MAP(GEN) \
    GEN(HEADER_CACHE_CONTROL, "cache-control") \
    GEN(HEADER_CONNECTION, "connection") \
    GEN(HEADER_DATE, "date") \
    GEN(HEADER_PRAGMA, "pragma") \
    GEN(HEADER_TRAILER, "trailer") \
    GEN(HEADER_TRANSFER_ENCODING, "transfer-encoding") \
    GEN(HEADER_UPGRADE, "upgrade") \
    GEN(HEADER_VIA, "via") \
    GEN(HEADER_WARNING, "warning") \
    GEN(HEADER_ALLOW, "allow") \
    GEN(HEADER_CONTENT_ENCODING, "content-encoding") \
    GEN(HEADER_CONTENT_LANGUAGE, "content-language") \
    GEN(HEADER_CONTENT_LENGTH, "content-length") \
    GEN(HEADER_CONTENT_LOCATION, "content-location") \
    GEN(HEADER_CONTENT_MD5, "content-md5") \
    GEN(HEADER_CONTENT_RANGE, "content-range") \
    GEN(HEADER_CONTENT_TYPE, "content-type") \
    GEN(HEADER_EXPIRES, "expires") \
    GEN(HEADER_LAST_MODIFIED, "last-modified") \
    GEN(HEADER_ACCEPT, "accept") \
    GEN(HEADER_ACCEPT_CHARSET, "accept-charset") \
    GEN(HEADER_ACCEPT_ENCODING, "accept-encoding") \
    GEN(HEADER_ACCEPT_LANGUAGE, "accept-language") \
    GEN(HEADER_AUTHORIZATION, "authorization") \
    GEN(HEADER_EXPECT, "expect") \
    GEN(HEADER_FROM, "from") \
    GEN(HEADER_HOST, "host") \
    GEN(HEADER_IF_MODIFIED_SINCE, "if-modified-since") \
    GEN(HEADER_IF_MATCH, "if-match") \
    GEN(HEADER_IF_NONE_MATCH, "if-none-match") \
    GEN(HEADER_IF_RANGE, "if-range") \
    GEN(HEADER_IF_UNMODIFIED_SINCE, "if-unmodified-since") \
    GEN(HEADER_MAX_FORWARDS, "max-forwards") \
    GEN(HEADER_PROXY_AUTHORIZATION, "proxy-authorization") \
    GEN(HEADER_RANGE, "range") \
    GEN(HEADER_REFERER, "referer") \
    GEN(HEADER_TE, "te") \
    GEN(HEADER_USER_AGENT, "user-agent") \
    GEN(HEADER_COOKIE, "cookie") \
    GEN(HEADER_ORIGIN, "origin") \
    GEN(HEADER_ACCESS_CONTROL_REQUEST_METHOD, \
        "access-control-request-method") \
    GEN(HEADER_ACCESS_CONTROL_REQUEST_HEADERS, \
        "access-control-request-headers")

LIBNODE_HTTP_SERVER_REQUEST_HEADER_MAP(
    LIBNODE_SERVER_REQUEST_HEADER_DEF_GEN)

const String::CPtr ServerRequestImpl::METHOD =
    String::create("method");
const String::CPtr ServerRequestImpl::URL =
    String::create("url");
const String::CPtr ServerRequestImpl::HEADERS =
    String::create("headers");
const String::CPtr ServerRequestImpl::HTTP_VERSION =
    String::create("httpVerion");

}  // namespace http
}  // namespace node
}  // namespace libj
