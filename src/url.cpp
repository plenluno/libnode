// Copyright (c) 2012 Plenluno All rights reserved.

#include <url_parser.h>

#include "libnode/url.h"

namespace libj {
namespace node {
namespace url {

const String::CPtr HREF = String::create("href");
const String::CPtr PROTOCOL = String::create("protocol");
const String::CPtr HOST = String::create("host");
const String::CPtr AUTH = String::create("auth");
const String::CPtr HOSTNAME = String::create("hostname");
const String::CPtr PORT = String::create("port");
const String::CPtr PATHNAME = String::create("pathname");
const String::CPtr SEARCH = String::create("search");
const String::CPtr PATH = String::create("path");
const String::CPtr QUERY = String::create("query");
const String::CPtr HASH = String::create("hash");

JsObject::Ptr parse(String::CPtr urlStr) {
    static const String::CPtr colon = String::create(":");
    static const String::CPtr slash = String::create("/");
    static const String::CPtr sharp = String::create("#");
    static const String::CPtr question = String::create("?");

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
            obj->put(HOST, hostname->concat(colon)->concat(port));
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
        String::CPtr pathname = sharp->concat(String::create(url->path));
        obj->put(PATHNAME, pathname);
        if (query) {
            obj->put(PATH, pathname->concat(question)->concat(query));
        } else {
            obj->put(PATH, pathname);
        }
    }
    if (url->username && url->password) {
        String::CPtr auth = String::create(url->username);
        auth = auth->concat(colon);
        auth = auth->concat(String::create(url->password));
        obj->put(AUTH, auth);
    }
    if (url->fragment) {
        String::CPtr hash = sharp->concat(String::create(url->fragment));
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
