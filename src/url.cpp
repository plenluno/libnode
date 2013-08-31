// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/url.h>

#include <libj/string_buffer.h>

#include <assert.h>
#include <http_parser.h>

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

static String::CPtr getField(
    const char* url,
    http_parser_url_fields field,
    const http_parser_url* info) {
    assert(url && info);
    if (info->field_set & (1 << field)) {
        Size offset = info->field_data[field].off;
        Size length = info->field_data[field].len;
        return String::create(url + offset, String::UTF8, length);
    } else {
        return String::null();
    }
}

JsObject::Ptr parse(String::CPtr urlStr) {
    if (!urlStr) return JsObject::null();

    http_parser_url info;
    std::string str = urlStr->toStdString();
    const char* cstr = str.c_str();
    int r = http_parser_parse_url(
        cstr,
        str.length(),
        0,
        &info);
    if (r) return JsObject::null();

    String::CPtr scheme   = getField(cstr, UF_SCHEMA,   &info);
    String::CPtr host     = getField(cstr, UF_HOST,     &info);
    String::CPtr port     = getField(cstr, UF_PORT,     &info);
    String::CPtr path     = getField(cstr, UF_PATH,     &info);
    String::CPtr query    = getField(cstr, UF_QUERY,    &info);
    String::CPtr fragment = getField(cstr, UF_FRAGMENT, &info);
    String::CPtr userinfo = getField(cstr, UF_USERINFO, &info);

    JsObject::Ptr urlObj = JsObject::create();

    // TODO(plenluno): protocol and host are lowercased
    urlObj->put(HREF, urlStr);

    if (scheme) {
        scheme = scheme->toLowerCase();
        StringBuilder::Ptr sb = StringBuilder::create();
        sb->appendStr(scheme);
        sb->appendChar(':');
        urlObj->put(PROTOCOL, sb->toString());
    }

    if (host) {
        host = host->toLowerCase();
        urlObj->put(HOSTNAME, host);

        if (port) {
            StringBuilder::Ptr sb = StringBuilder::create();
            sb->appendStr(host);
            sb->appendChar(':');
            sb->appendStr(port);
            urlObj->put(HOST, sb->toString());
        } else {
            urlObj->put(HOST, host);
        }
    }

    if (port) {
        urlObj->put(PORT, port);
    }

    if (path) {
        urlObj->put(PATHNAME, path);

        if (query) {
            StringBuilder::Ptr sb = StringBuilder::create();
            sb->appendStr(path);
            sb->appendChar('?');
            sb->appendStr(query);
            urlObj->put(PATH, sb->toString());
        } else {
            urlObj->put(PATH, path);
        }
    }

    if (query) {
        urlObj->put(QUERY, query);

        StringBuilder::Ptr sb = StringBuilder::create();
        sb->appendChar('?');
        sb->appendStr(query);
        urlObj->put(SEARCH, sb->toString());
    }

    if (fragment) {
        StringBuilder::Ptr sb = StringBuilder::create();
        sb->appendChar('#');
        sb->appendStr(fragment);
        urlObj->put(HASH, sb->toString());
    }

    if (userinfo) {
        urlObj->put(AUTH, userinfo);
    }

    return urlObj;
}

String::CPtr format(JsObject::CPtr urlObj) {
    // TODO(plenluno): implement
    return String::create();
}

}  // namespace url
}  // namespace node
}  // namespace libj
