// Copyright (c) 2012 Plenluno All rights reserved.

#include <url_parser.h>

#include "libnode/url.h"

namespace libj {
namespace node {
namespace url {

String::CPtr HREF = String::create("href");
String::CPtr PROTOCOL = String::create("protocol");
String::CPtr HOST = String::create("host");
String::CPtr AUTH = String::create("auth");
String::CPtr HOSTNAME = String::create("hostname");
String::CPtr PORT = String::create("port");
String::CPtr PATHNAME = String::create("pathname");
String::CPtr SEARCH = String::create("search");
String::CPtr PATH = String::create("path");
String::CPtr QUERY = String::create("query");
String::CPtr HASH = String::create("hash");

static String::CPtr COLON = String::create(":");
static String::CPtr SLASH = String::create("/");
static String::CPtr SHARP = String::create("#");
static String::CPtr QUESTION = String::create("?");

JsObject::Ptr parse(String::CPtr urlStr) {
    if (!urlStr) {
        LIBJ_NULL_PTR(JsObject, nullp);
        return nullp;
    }

    struct parsed_url* url = parse_url(urlStr->toStdString().c_str());
    JsObject::Ptr obj = JsObject::create();

    obj->put(HREF, urlStr);
    if (url->scheme) {
        obj->put(PROTOCOL, String::create(url->scheme)->toLowerCase());
    }
    LIBJ_NULL_CPTR(String, port);
    if (url->port) {
        port = String::create(url->port);
        obj->put(PORT, port);
    }
    if (url->host) {
        String::CPtr hostname = String::create(url->host)->toLowerCase();
        obj->put(HOSTNAME, hostname);
        if (port) {
            obj->put(HOST, hostname->concat(COLON)->concat(port));
        } else {
            obj->put(HOST, hostname);
        }
    }
    LIBJ_NULL_CPTR(String, query);
    if (url->query) {
        query = String::create(url->query);
        obj->put(QUERY, query);
    }
    if (url->path) {
        String::CPtr pathname = SLASH->concat(String::create(url->path));
        obj->put(PATHNAME, pathname);
        if (query) {
            obj->put(PATH, pathname->concat(QUESTION)->concat(query));
        } else {
            obj->put(PATH, pathname);
        }
    }
    if (url->username && url->password) {
        String::CPtr auth = String::create(url->username);
        auth = auth->concat(COLON);
        auth = auth->concat(String::create(url->password));
        obj->put(AUTH, auth);
    }
    if (url->fragment) {
        String::CPtr hash = SHARP->concat(String::create(url->fragment));
        obj->put(HASH, hash);
    }

    parsed_url_free(url);
    return obj;
}

String::CPtr format(JsObject::CPtr urlObj) {
    // TODO(plenluno): implement
    return String::create();
}

}  // namespace url
}  // namespace node
}  // namespace libj
