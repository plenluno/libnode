// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_status.h"

namespace libj {
namespace node {
namespace http {

class StatusImpl : public Status {
 private:
    static const String::CPtr MSG_CONTINUE;
    static const String::CPtr MSG_SWITCHING_PROTOCOLS;
    static const String::CPtr MSG_OK;
    static const String::CPtr MSG_CREATED;
    static const String::CPtr MSG_ACCEPTED;
    static const String::CPtr MSG_NON_AUTHORITATIVE_INFORMATION;
    static const String::CPtr MSG_NO_CONTENT;
    static const String::CPtr MSG_RESET_CONTENT;
    static const String::CPtr MSG_PARTIAL_CONTENT;
    static const String::CPtr MSG_MULTIPLE_CHOICES;
    static const String::CPtr MSG_MOVED_PERMANENTLY;
    static const String::CPtr MSG_FOUND;
    static const String::CPtr MSG_SEE_OTHER;
    static const String::CPtr MSG_NOT_MODIFIED;
    static const String::CPtr MSG_USE_PROXY;
    static const String::CPtr MSG_TEMPORARY_REDIRECT;
    static const String::CPtr MSG_BAD_REQUEST;
    static const String::CPtr MSG_UNAUTHORIZED;
    static const String::CPtr MSG_PAYMENT_REQUIRED;
    static const String::CPtr MSG_FORBIDDEN;
    static const String::CPtr MSG_NOT_FOUND;
    static const String::CPtr MSG_METHOD_NOT_ALLOWED;
    static const String::CPtr MSG_NOT_ACCEPTABLE;
    static const String::CPtr MSG_PROXY_AUTHENTICATION_REQUIRED;
    static const String::CPtr MSG_REQUEST_TIMEOUT;
    static const String::CPtr MSG_CONFLICT;
    static const String::CPtr MSG_GONE;
    static const String::CPtr MSG_LENGTH_REQUIRED;
    static const String::CPtr MSG_PRECONDITION_FAILED;
    static const String::CPtr MSG_REQUEST_ENTITY_TOO_LARGE;
    static const String::CPtr MSG_REQUEST_URI_TOO_LONG;
    static const String::CPtr MSG_UNSUPPORTED_MEDIA_TYPE;
    static const String::CPtr MSG_REQUESTED_RANGE_NOT_SATISFIABLE;
    static const String::CPtr MSG_EXPECTATION_FAILED;
    static const String::CPtr MSG_INTERNAL_SERVER_ERROR;
    static const String::CPtr MSG_NOT_IMPLEMENTED;
    static const String::CPtr MSG_BAD_GATEWAY;
    static const String::CPtr MSG_SERVICE_UNAVAILABLE;
    static const String::CPtr MSG_GATEWAY_TIMEOUT;
    static const String::CPtr MSG_HTTP_VERSION_NOT_SUPPORTED;

 private:
    libj::Status::CPtr status_;

    StatusImpl(Int code, String::CPtr msg)
        : status_(libj::Status::create(code, msg)) {}

 public:
    static CPtr create(Int code) {
        if (code < 200 || code >= 600) {
            LIBJ_NULL_CPTR(Status, nullp);
            return nullp;
        }

        String::CPtr msg;
        switch (code) {
        case CONTINUE:
            msg = MSG_CONTINUE;
            break;
        case SWITCHING_PROTOCOLS:
            msg = MSG_SWITCHING_PROTOCOLS;
            break;
        case OK:
            msg = MSG_OK;
            break;
        case CREATED:
            msg = MSG_CREATED;
            break;
        case ACCEPTED:
            msg = MSG_ACCEPTED;
            break;
        case NON_AUTHORITATIVE_INFORMATION:
            msg = MSG_NON_AUTHORITATIVE_INFORMATION;
            break;
        case NO_CONTENT:
            msg = MSG_NO_CONTENT;
            break;
        case RESET_CONTENT:
            msg = MSG_RESET_CONTENT;
            break;
        case PARTIAL_CONTENT:
            msg = MSG_PARTIAL_CONTENT;
            break;
        case MULTIPLE_CHOICES:
            msg = MSG_MULTIPLE_CHOICES;
            break;
        case MOVED_PERMANENTLY:
            msg = MSG_MOVED_PERMANENTLY;
            break;
        case FOUND:
            msg = MSG_FOUND;
            break;
        case SEE_OTHER:
            msg = MSG_SEE_OTHER;
            break;
        case NOT_MODIFIED:
            msg = MSG_NOT_MODIFIED;
            break;
        case USE_PROXY:
            msg = MSG_USE_PROXY;
            break;
        case TEMPORARY_REDIRECT:
            msg = MSG_TEMPORARY_REDIRECT;
            break;
        case BAD_REQUEST:
            msg = MSG_BAD_REQUEST;
            break;
        case UNAUTHORIZED:
            msg = MSG_UNAUTHORIZED;
            break;
        case PAYMENT_REQUIRED:
            msg = MSG_PAYMENT_REQUIRED;
            break;
        case FORBIDDEN:
            msg = MSG_FORBIDDEN;
            break;
        case NOT_FOUND:
            msg = MSG_NOT_FOUND;
            break;
        case METHOD_NOT_ALLOWED:
            msg = MSG_METHOD_NOT_ALLOWED;
            break;
        case NOT_ACCEPTABLE:
            msg = MSG_NOT_ACCEPTABLE;
            break;
        case PROXY_AUTHENTICATION_REQUIRED:
            msg = MSG_PROXY_AUTHENTICATION_REQUIRED;
            break;
        case REQUEST_TIMEOUT:
            msg = MSG_REQUEST_TIMEOUT;
            break;
        case CONFLICT:
            msg = MSG_CONFLICT;
            break;
        case GONE:
            msg = MSG_GONE;
            break;
        case LENGTH_REQUIRED:
            msg = MSG_LENGTH_REQUIRED;
            break;
        case PRECONDITION_FAILED:
            msg = MSG_PRECONDITION_FAILED;
            break;
        case REQUEST_ENTITY_TOO_LARGE:
            msg = MSG_REQUEST_ENTITY_TOO_LARGE;
            break;
        case REQUEST_URI_TOO_LONG:
            msg = MSG_REQUEST_URI_TOO_LONG;
            break;
        case UNSUPPORTED_MEDIA_TYPE:
            msg = MSG_UNSUPPORTED_MEDIA_TYPE;
            break;
        case REQUESTED_RANGE_NOT_SATISFIABLE:
            msg = MSG_REQUESTED_RANGE_NOT_SATISFIABLE;
            break;
        case EXPECTATION_FAILED:
            msg = MSG_EXPECTATION_FAILED;
            break;
        case INTERNAL_SERVER_ERROR:
            msg = MSG_INTERNAL_SERVER_ERROR;
            break;
        case NOT_IMPLEMENTED:
            msg = MSG_NOT_IMPLEMENTED;
            break;
        case BAD_GATEWAY:
            msg = MSG_BAD_GATEWAY;
            break;
        case SERVICE_UNAVAILABLE:
            msg = MSG_SERVICE_UNAVAILABLE;
            break;
        case GATEWAY_TIMEOUT:
            msg = MSG_GATEWAY_TIMEOUT;
            break;
        case HTTP_VERSION_NOT_SUPPORTED:
            msg = MSG_HTTP_VERSION_NOT_SUPPORTED;
            break;
        default:
            LIBJ_NULL_CPTR(String, nullp);
            msg = nullp;
        }
        CPtr p(new StatusImpl(code, msg));
        return p;
    }

    static CPtr create(Int code, String::CPtr msg) {
        if (code < 200 || code >= 600) {
            LIBJ_NULL_CPTR(Status, nullp);
            return nullp;
        } else {
            CPtr p(new StatusImpl(code, msg));
            return p;
        }
    }

    LIBJ_STATUS_IMPL(status_);
};

const String::CPtr StatusImpl::MSG_CONTINUE =
    String::create("Continue");
const String::CPtr StatusImpl::MSG_SWITCHING_PROTOCOLS =
    String::create("Switching Protocols");
const String::CPtr StatusImpl::MSG_OK =
    String::create("OK");
const String::CPtr StatusImpl::MSG_CREATED =
    String::create("Created");
const String::CPtr StatusImpl::MSG_ACCEPTED =
    String::create("Accepted");
const String::CPtr StatusImpl::MSG_NON_AUTHORITATIVE_INFORMATION =
    String::create("Non-Authoritative Information");
const String::CPtr StatusImpl::MSG_NO_CONTENT =
    String::create("No Content");
const String::CPtr StatusImpl::MSG_RESET_CONTENT =
    String::create("Reset Content");
const String::CPtr StatusImpl::MSG_PARTIAL_CONTENT =
    String::create("Partial Content");
const String::CPtr StatusImpl::MSG_MULTIPLE_CHOICES =
    String::create("Multiple Choices");
const String::CPtr StatusImpl::MSG_MOVED_PERMANENTLY =
    String::create("Moved Permanently");
const String::CPtr StatusImpl::MSG_FOUND =
    String::create("Found");
const String::CPtr StatusImpl::MSG_SEE_OTHER =
    String::create("See Other");
const String::CPtr StatusImpl::MSG_NOT_MODIFIED =
    String::create("Not Modified");
const String::CPtr StatusImpl::MSG_USE_PROXY =
    String::create("Use Proxy");
const String::CPtr StatusImpl::MSG_TEMPORARY_REDIRECT =
    String::create("Temporary Redirect");
const String::CPtr StatusImpl::MSG_BAD_REQUEST =
    String::create("Bad Request");
const String::CPtr StatusImpl::MSG_UNAUTHORIZED =
    String::create("Unauthorized");
const String::CPtr StatusImpl::MSG_PAYMENT_REQUIRED =
    String::create("Payment Required");
const String::CPtr StatusImpl::MSG_FORBIDDEN =
    String::create("Forbidden");
const String::CPtr StatusImpl::MSG_NOT_FOUND =
    String::create("Not Found");
const String::CPtr StatusImpl::MSG_METHOD_NOT_ALLOWED =
    String::create("Method Not Allowed");
const String::CPtr StatusImpl::MSG_NOT_ACCEPTABLE =
    String::create("Not Acceptable");
const String::CPtr StatusImpl::MSG_PROXY_AUTHENTICATION_REQUIRED =
    String::create("Proxy Authentication Required");
const String::CPtr StatusImpl::MSG_REQUEST_TIMEOUT =
    String::create("Request Timeout");
const String::CPtr StatusImpl::MSG_CONFLICT =
    String::create("Conflict");
const String::CPtr StatusImpl::MSG_GONE =
    String::create("Gone");
const String::CPtr StatusImpl::MSG_LENGTH_REQUIRED =
    String::create("Length Required");
const String::CPtr StatusImpl::MSG_PRECONDITION_FAILED =
    String::create("Precondition Failed");
const String::CPtr StatusImpl::MSG_REQUEST_ENTITY_TOO_LARGE =
    String::create("Request Entity Too Large");
const String::CPtr StatusImpl::MSG_REQUEST_URI_TOO_LONG =
    String::create("Request-URI Too Long");
const String::CPtr StatusImpl::MSG_UNSUPPORTED_MEDIA_TYPE =
    String::create("Unsupported Media Type");
const String::CPtr StatusImpl::MSG_REQUESTED_RANGE_NOT_SATISFIABLE =
    String::create("Requested Range Not Satisfiable");
const String::CPtr StatusImpl::MSG_EXPECTATION_FAILED =
    String::create("Expectation Failed");
const String::CPtr StatusImpl::MSG_INTERNAL_SERVER_ERROR =
    String::create("Internal Server Error");
const String::CPtr StatusImpl::MSG_NOT_IMPLEMENTED =
    String::create("Not Implemented");
const String::CPtr StatusImpl::MSG_BAD_GATEWAY =
    String::create("Bad Gateway");
const String::CPtr StatusImpl::MSG_SERVICE_UNAVAILABLE =
    String::create("Service Unavailable");
const String::CPtr StatusImpl::MSG_GATEWAY_TIMEOUT =
    String::create("Gateway Timeout");
const String::CPtr StatusImpl::MSG_HTTP_VERSION_NOT_SUPPORTED =
    String::create("HTTP Version Not Supported");

Status::CPtr Status::create(Int code) {
    return StatusImpl::create(code);
}

Status::CPtr Status::create(Int code, String::CPtr msg) {
    return StatusImpl::create(code, msg);
}

}  // namespace http
}  // namespace node
}  // namespace libj
