// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/url.h>

#include <url_parser.h>
#include <libj/string_buffer.h>

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
    if (!urlStr) {
        return JsObject::null();
    }

    struct parsed_url* url = parse_url(urlStr->toStdString().c_str());
    JsObject::Ptr obj = JsObject::create();

    obj->put(HREF, urlStr);
    if (url->scheme) {
        StringBuffer::Ptr protocol = StringBuffer::create();
        protocol->appendCStr(url->scheme);
        protocol->appendChar(':');
        obj->put(PROTOCOL, protocol->toString()->toLowerCase());
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
            StringBuffer::Ptr sb = StringBuffer::create();
            sb->append(hostname);
            sb->appendChar(':');
            sb->append(port);
            obj->put(HOST, sb->toString());
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
        StringBuffer::Ptr sb = StringBuffer::create();
        sb->appendChar('/');
        sb->appendCStr(url->path);
        String::CPtr pathname = sb->toString();
        obj->put(PATHNAME, pathname);
        if (query) {
            sb->appendChar('?');
            sb->append(query);
            obj->put(PATH, sb->toString());
        } else {
            obj->put(PATH, pathname);
        }
    }
    if (url->username && url->password) {
        StringBuffer::Ptr sb = StringBuffer::create();
        sb->appendCStr(url->username);
        sb->appendChar(':');
        sb->appendCStr(url->password);
        obj->put(AUTH, sb->toString());
    }
    if (url->fragment) {
        StringBuffer::Ptr sb = StringBuffer::create();
        sb->appendChar('#');
        sb->appendCStr(url->fragment);
        obj->put(HASH, sb->toString());
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
