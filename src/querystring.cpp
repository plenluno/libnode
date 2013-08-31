// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/querystring.h>
#include <libnode/util.h>

#include <libj/js_array.h>
#include <libj/string_builder.h>

#include <assert.h>

namespace libj {
namespace node {
namespace querystring {

static void addPair(JsObject::Ptr obj, String::CPtr key, String::CPtr val) {
    if (obj->containsKey(key)) {
        Value v = obj->get(key);
        if (v.is<String>()) {
            JsArray::Ptr ary = JsArray::create();
            ary->add(obj->get(key));
            ary->add(val);
            obj->put(key, ary);
        } else {
            JsArray::Ptr ary = toPtr<JsArray>(v);
            assert(ary);
            ary->add(val);
            obj->put(key, ary);
        }
    } else {
        obj->put(key, val);
    }
}

JsObject::Ptr parse(String::CPtr query, Char sep, Char eq) {
    JsObject::Ptr obj = JsObject::create();
    if (!query || query->length() == 0) {
        return obj;
    }

    Size keyStart = 0;
    Size valStart = 0;
    String::CPtr null = String::null();
    String::CPtr key = null;
    String::CPtr val = null;
    Size len = query->length();
    for (Size i = 0; i < len; i++) {
        if (query->charAt(i) == eq) {
            key = query->substring(keyStart, i);
            valStart = i + 1;
        } else if (query->charAt(i) == sep) {
            if (key) {
                val = query->substring(valStart, i);
            } else {
                key = query->substring(keyStart, i);
                val = String::create();
            }
            addPair(obj,
                util::percentDecode(key),
                util::percentDecode(val));
            keyStart = i + 1;
            key = null;
            val = null;
        }
    }
    if (key) {
        val = query->substring(valStart);
    } else {
        key = query->substring(keyStart);
        val = String::create();
    }
    addPair(obj,
        util::percentDecode(key),
        util::percentDecode(val));
    return obj;
}

static String::CPtr toString(Value val) {
    if (val.isNull() ||
        val.isUndefined() ||
        val.is<JsArray>() ||
        val.is<JsObject>()) {
        return String::create();
    } else {
        return util::percentEncode(String::valueOf(val));
    }
}

String::CPtr stringify(JsObject::CPtr obj, Char sep, Char eq) {
    if (!obj) return String::create();

    StringBuilder::Ptr res = StringBuilder::create();
    typedef JsObject::Entry Entry;
    TypedSet<Entry::CPtr>::CPtr entrys = obj->entrySet();
    TypedIterator<Entry::CPtr>::Ptr itr = entrys->iteratorTyped();
    Boolean first = true;
    while (itr->hasNext()) {
        if (first) {
            first = false;
        } else {
            res->appendChar(sep);
        }
        Entry::CPtr entry = itr->nextTyped();
        Value key = entry->getKey();
        Value val = entry->getValue();
        if (val.is<JsArray>()) {
            JsArray::CPtr ary = toCPtr<JsArray>(val);
            for (Size i = 0; i < ary->length(); i++) {
                if (i) {
                    res->appendChar(sep);
                }
                res->appendStr(toString(key));
                res->appendChar(eq);
                res->appendStr(toString(ary->get(i)));
            }
        } else {
            res->appendStr(toString(key));
            res->appendChar(eq);
            res->appendStr(toString(val));
        }
    }
    return res->toString();
}

}  // namespace querystring
}  // namespace node
}  // namespace libj
