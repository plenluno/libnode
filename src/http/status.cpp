// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http/status.h"

namespace libj {
namespace node {
namespace http {

#define LIBNODE_HTTP_STATUS_MSG_MAP(GEN) \
    GEN(CONTINUE, "Continue") \
    GEN(SWITCHING_PROTOCOLS, "Switching Protocols") \
    GEN(OK, "OK") \
    GEN(CREATED, "Created") \
    GEN(ACCEPTED, "Accepted") \
    GEN(NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information") \
    GEN(NO_CONTENT, "No Content") \
    GEN(RESET_CONTENT, "Reset Content") \
    GEN(PARTIAL_CONTENT, "Partial Content") \
    GEN(MULTIPLE_CHOICES, "Multiple Choices") \
    GEN(MOVED_PERMANENTLY, "Moved Permanently") \
    GEN(FOUND, "Found") \
    GEN(SEE_OTHER, "See Other") \
    GEN(NOT_MODIFIED, "Not Modified") \
    GEN(USE_PROXY, "Use Proxy") \
    GEN(TEMPORARY_REDIRECT, "Temporary Redirect") \
    GEN(BAD_REQUEST, "Bad Request") \
    GEN(UNAUTHORIZED, "Unauthorized") \
    GEN(PAYMENT_REQUIRED, "Payment Required") \
    GEN(FORBIDDEN, "Forbidden") \
    GEN(NOT_FOUND, "Not Found") \
    GEN(METHOD_NOT_ALLOWED, "Method Not Allowed") \
    GEN(NOT_ACCEPTABLE, "Not Acceptable") \
    GEN(PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required") \
    GEN(REQUEST_TIMEOUT, "Request Timeout") \
    GEN(CONFLICT, "Conflict") \
    GEN(GONE, "Gone") \
    GEN(LENGTH_REQUIRED, "Length Required") \
    GEN(PRECONDITION_FAILED, "Precondition Failed") \
    GEN(REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large") \
    GEN(REQUEST_URI_TOO_LONG, "Request-URI Too Long") \
    GEN(UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type") \
    GEN(REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable") \
    GEN(EXPECTATION_FAILED, "Expectation Failed") \
    GEN(INTERNAL_SERVER_ERROR, "Internal Server Error") \
    GEN(NOT_IMPLEMENTED, "Not Implemented") \
    GEN(BAD_GATEWAY, "Bad Gateway") \
    GEN(SERVICE_UNAVAILABLE, "Service Unavailable") \
    GEN(GATEWAY_TIMEOUT, "Gateway Timeout") \
    GEN(HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported")

#define LIBNODE_HTTP_STATUS_MSG_DECL_GEN(NAME, MESSAGE) \
    static const String::CPtr MSG_##NAME;

#define LIBNODE_HTTP_STATUS_MSG_CASE_GEN(NAME, MESSAGE) \
    case NAME: \
        msg = MSG_##NAME; \
        break;

class StatusImpl : public Status {
 private:
    LIBNODE_HTTP_STATUS_MSG_MAP(LIBNODE_HTTP_STATUS_MSG_DECL_GEN)

 private:
    libj::Status::CPtr status_;

    StatusImpl(Int code, String::CPtr msg)
        : status_(libj::Status::create(code, msg)) {}

 public:
    static CPtr create(Int code) {
        if (code < 100 || code >= 600) {
            return null();
        }

        String::CPtr msg;
        switch (code) {
            LIBNODE_HTTP_STATUS_MSG_MAP(LIBNODE_HTTP_STATUS_MSG_CASE_GEN)
        default:
            msg = String::null();
        }
        CPtr p(new StatusImpl(code, msg));
        return p;
    }

    static CPtr create(Int code, String::CPtr msg) {
        if (code < 100 || code >= 600) {
            return null();
        } else {
            CPtr p(new StatusImpl(code, msg));
            return p;
        }
    }

    LIBJ_STATUS_IMPL(status_);
};

#define LIBNODE_HTTP_STATUS_MSG_DEF_GEN(NAME, MESSAGE) \
    const String::CPtr StatusImpl::MSG_##NAME = String::create(MESSAGE);

LIBNODE_HTTP_STATUS_MSG_MAP(LIBNODE_HTTP_STATUS_MSG_DEF_GEN)

Status::CPtr Status::create(Int code) {
    return StatusImpl::create(code);
}

Status::CPtr Status::create(Int code, String::CPtr msg) {
    return StatusImpl::create(code, msg);
}

}  // namespace http
}  // namespace node
}  // namespace libj
