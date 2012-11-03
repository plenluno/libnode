// Copyright (c) 2012 Plenluno All rights reserved.

#include <url_parser.h>

#include "libnode/url.h"

namespace libj {
namespace node {
namespace url {

LIBJ_SYMBOL_DEF(HREF,     "href");
LIBJ_SYMBOL_DEF(PROTOCOL, "protocol");
LIBJ_SYMBOL_DEF(HOST,     "host");
LIBJ_SYMBOL_DEF(AUTH,     "auth");
LIBJ_SYMBOL_DEF(HOSTNAME, "hostname");
LIBJ_SYMBOL_DEF(PORT,     "port");
LIBJ_SYMBOL_DEF(PATHNAME, "pathname");
LIBJ_SYMBOL_DEF(SEARCH,   "search");
LIBJ_SYMBOL_DEF(PATH,     "path");
LIBJ_SYMBOL_DEF(QUERY,    "query");
LIBJ_SYMBOL_DEF(HASH,     "hash");

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
