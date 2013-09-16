// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/os.h>
#include <libnode/net.h>

#include "./gtest_common.h"

namespace libj {
namespace node {

TEST(GTestOs, TestOsInfo) {
    String::CPtr hostname = os::hostname();
    String::CPtr type = os::type();
    String::CPtr release = os::release();

    ASSERT_TRUE(!!hostname);
    ASSERT_TRUE(!!type);
    ASSERT_TRUE(!!release);

    console::log(hostname);
    console::log(type);
    console::log(release);
}

TEST(GTestOs, TestNetworkInterface) {
    LIBJ_STATIC_SYMBOL_DEF(symIPv4,     "IPv4");
    LIBJ_STATIC_SYMBOL_DEF(symIPv6,     "IPv6");
    LIBJ_STATIC_SYMBOL_DEF(symFamily,   "family");
    LIBJ_STATIC_SYMBOL_DEF(symAddress,  "address");
    LIBJ_STATIC_SYMBOL_DEF(symInternal, "internal");

    JsObject::Ptr netif = os::networkInterface();
    console::log(json::stringify(netif));

    TypedSet<Map::Entry::CPtr>::CPtr es = netif->entrySet();
    TypedIterator<Map::Entry::CPtr>::Ptr itr = es->iteratorTyped();
    while (itr->hasNext()) {
        Map::Entry::CPtr e = itr->nextTyped();
        JsArray::Ptr a = toPtr<JsArray>(e->getValue());
        ASSERT_TRUE(!!a);
        for (Size i = 0; i < a->length(); i++) {
            JsObject::Ptr o = a->getPtr<JsObject>(i);
            String::CPtr family = o->getCPtr<String>(symFamily);
            ASSERT_TRUE(family->equals(symIPv4) || family->equals(symIPv6));
            if (family->equals(symIPv4)) {
                ASSERT_TRUE(net::isIPv4(o->getCPtr<String>(symAddress)));
            } else {
                ASSERT_TRUE(net::isIPv6(o->getCPtr<String>(symAddress)));
            }
            ASSERT_TRUE(o->get(symInternal).is<Boolean>());
        }
    }
}

}  // namespace node
}  // namespace libj
