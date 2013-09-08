// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_HEADER_SCANNER_H_
#define LIBNODE_DETAIL_HTTP_HEADER_SCANNER_H_

#include <libnode/http/header.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

#define YYLIMIT     limit
#define YYCURSOR    cursor
#define YYMARKER    marker
#define YYFILL(n)   NULL

template<typename YYCTYPE>
inline String::CPtr createLowerString(const YYCTYPE* buf, size_t len);

template<>
inline String::CPtr createLowerString(const char* buf, size_t len) {
    return String::create(buf, String::UTF8, len)->toLowerCase();
}

template<>
inline String::CPtr createLowerString(const char16_t* buf, size_t len) {
    return String::create(buf, String::UTF16, len)->toLowerCase();
}

template<>
inline String::CPtr createLowerString(const char32_t* buf, size_t len) {
    return String::create(buf, String::UTF32, len)->toLowerCase();
}

template<typename YYCTYPE>
inline String::CPtr scanHeaderField(const YYCTYPE* buf, size_t len) {
    if (!buf || !len) return String::create();

    const YYCTYPE* limit  = buf;
    const YYCTYPE* cursor = buf;
    const YYCTYPE* marker = NULL;

/*!re2c
"Cache-Control"                     { return node::http::LHEADER_CACHE_CONTROL; }
"Connection"                        { return node::http::LHEADER_CONNECTION; }
"Date"                              { return node::http::LHEADER_DATE; }
"Pragma"                            { return node::http::LHEADER_PRAGMA; }
"Trailer"                           { return node::http::LHEADER_TRAILER; }
"Transfer-Encoding"                 { return node::http::LHEADER_TRANSFER_ENCODING; }
"Upgrade"                           { return node::http::LHEADER_UPGRADE; }
"Via"                               { return node::http::LHEADER_VIA; }
"Warning"                           { return node::http::LHEADER_WARNING; }
"Allow"                             { return node::http::LHEADER_ALLOW; }
"Content-Encoding"                  { return node::http::LHEADER_CONTENT_ENCODING; }
"Content-Language"                  { return node::http::LHEADER_CONTENT_LANGUAGE; }
"Content-Length"                    { return node::http::LHEADER_CONTENT_LENGTH; }
"Content-Location"                  { return node::http::LHEADER_CONTENT_LOCATION; }
"Content-MD5"                       { return node::http::LHEADER_CONTENT_MD5; }
"Content-Range"                     { return node::http::LHEADER_CONTENT_RANGE; }
"Content-Type"                      { return node::http::LHEADER_CONTENT_TYPE; }
"Expires"                           { return node::http::LHEADER_EXPIRES; }
"Last-Modified"                     { return node::http::LHEADER_LAST_MODIFIED; }
"Accept"                            { return node::http::LHEADER_ACCEPT; }
"Accept-Charset"                    { return node::http::LHEADER_ACCEPT_CHARSET; }
"Accept-Encoding"                   { return node::http::LHEADER_ACCEPT_ENCODING; }
"Accept-Language"                   { return node::http::LHEADER_ACCEPT_LANGUAGE; }
"Authorization"                     { return node::http::LHEADER_AUTHORIZATION; }
"Expect"                            { return node::http::LHEADER_EXPECT; }
"From"                              { return node::http::LHEADER_FROM; }
"Host"                              { return node::http::LHEADER_HOST; }
"If-Modified-Since"                 { return node::http::LHEADER_IF_MODIFIED_SINCE; }
"If-Match"                          { return node::http::LHEADER_IF_MATCH; }
"If-None-Match"                     { return node::http::LHEADER_IF_NONE_MATCH; }
"If-Range"                          { return node::http::LHEADER_IF_RANGE; }
"If-Unmodified-Since"               { return node::http::LHEADER_IF_UNMODIFIED_SINCE; }
"Max-Forwards"                      { return node::http::LHEADER_MAX_FORWARDS; }
"Proxy-Authorization"               { return node::http::LHEADER_PROXY_AUTHORIZATION; }
"Range"                             { return node::http::LHEADER_RANGE; }
"Referer"                           { return node::http::LHEADER_REFERER; }
"TE"                                { return node::http::LHEADER_TE; }
"User-Agent"                        { return node::http::LHEADER_USER_AGENT; }
"Accept-Ranges"                     { return node::http::LHEADER_ACCEPT_RANGES; }
"Age"                               { return node::http::LHEADER_AGE; }
"ETag"                              { return node::http::LHEADER_ETAG; }
"Location"                          { return node::http::LHEADER_LOCATION; }
"Proxy-Authenticate"                { return node::http::LHEADER_PROXY_AUTHENTICATE; }
"Retry-After"                       { return node::http::LHEADER_RETRY_AFTER; }
"Server"                            { return node::http::LHEADER_SERVER; }
"Vary"                              { return node::http::LHEADER_VARY; }
"WWW-Authenticate"                  { return node::http::LHEADER_WWW_AUTHENTICATE; }
"Link"                              { return node::http::LHEADER_LINK; }
"Cookie"                            { return node::http::LHEADER_COOKIE; }
"Set-Cookie"                        { return node::http::LHEADER_SET_COOKIE; }
"Origin"                            { return node::http::LHEADER_ORIGIN; }
"Access-Control-Request-Method"     { return node::http::LHEADER_ACCESS_CONTROL_REQUEST_METHOD; }
"Access-Control-Request-Headers"    { return node::http::LHEADER_ACCESS_CONTROL_REQUEST_HEADERS; }
"Access-Control-Allow-Origin"       { return node::http::LHEADER_ACCESS_CONTROL_ALLOW_ORIGIN; }
"Access-Control-Allow-Credentials"  { return node::http::LHEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS; }
"Access-Control-Expose-Headers"     { return node::http::LHEADER_ACCESS_CONTROL_EXPOSE_HEADERS; }
"Access-Control-Max-Age"            { return node::http::LHEADER_ACCESS_CONTROL_MAX_AGE; }
"Access-Control-Allow-Methods"      { return node::http::LHEADER_ACCESS_CONTROL_ALLOW_METHODS; }
"Access-Control-Allow-Headers"      { return node::http::LHEADER_ACCESS_CONTROL_ALLOW_HEADERS; }
"Sec-WebSocket-Key"                 { return node::http::LHEADER_SEC_WEBSOCKET_KEY; }
"Sec-WebSocket-Extensions"          { return node::http::LHEADER_SEC_WEBSOCKET_EXTENSIONS; }
"Sec-WebSocket-Accept"              { return node::http::LHEADER_SEC_WEBSOCKET_ACCEPT; }
"Sec-WebSocket-Protocol"            { return node::http::LHEADER_SEC_WEBSOCKET_PROTOCOL; }
"Sec-WebSocket-Version"             { return node::http::LHEADER_SEC_WEBSOCKET_VERSION; }
"DAV"                               { return node::http::LHEADER_DAV; }
"Depth"                             { return node::http::LHEADER_DEPTH; }
"Destination"                       { return node::http::LHEADER_DESTINATION; }
"If"                                { return node::http::LHEADER_IF; }
"Lock-Token"                        { return node::http::LHEADER_LOCK_TOKEN; }
"Overwrite"                         { return node::http::LHEADER_OVERWRITE; }
"Status-URI"                        { return node::http::LHEADER_STATUS_URI; }
"Timeout"                           { return node::http::LHEADER_TIMEOUT; }
[^]                                 { return createLowerString(buf, len); }
*/
}

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_HEADER_SCANNER_H_
