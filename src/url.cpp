// Copyright (c) 2012 Plenluno All rights reserved.

#include <url_parser.h>

#include "libnode/url.h"

namespace libj {
namespace node {
namespace url {

const String::CPtr HREF = String::intern("href");
const String::CPtr PROTOCOL = String::intern("protocol");
const String::CPtr HOST = String::intern("host");
const String::CPtr AUTH = String::intern("auth");
const String::CPtr HOSTNAME = String::intern("hostname");
const String::CPtr PORT = String::intern("port");
const String::CPtr PATHNAME = String::intern("pathname");
const String::CPtr SEARCH = String::intern("search");
const String::CPtr PATH = String::intern("path");
const String::CPtr QUERY = String::intern("query");
const String::CPtr HASH = String::intern("hash");

JsObject::Ptr parse(String::CPtr urlStr) {
    static const String::CPtr colon = String::create(":");
    static const String::CPtr slash = String::create("/");
    static const String::CPtr sharp = String::create("#");
    static const String::CPtr question = String::create("?");

    if (!urlStr) {
        return JsObject::null();
    }

    struct parsed_url* url = parse_url(urlStr->toStdString().c_str());
    JsObject::Ptr obj = JsObject::create();

    obj->put(HREF, urlStr);
    if (url->scheme) {
        obj->put(PROTOCOL, String::create(url->scheme)->toLowerCase());
    }
    String::CPtr port = String::null();
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
    String::CPtr query = String::null();
    if (url->query) {
        query = String::create(url->query);
        obj->put(QUERY, query);
    }
    if (url->path) {
        String::CPtr pathname = slash->concat(String::create(url->path));
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
