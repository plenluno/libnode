// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/crypto.h>

namespace libj {
namespace node {
namespace crypto {

TEST(GTestCryptoHash, TestCreateHash) {
    ASSERT_TRUE(!!createHash(Hash::MD5));
    ASSERT_TRUE(!!createHash(Hash::SHA1));
    ASSERT_TRUE(!!createHash(Hash::SHA256));
    ASSERT_TRUE(!!createHash(Hash::SHA512));
}

TEST(GTestCryptoHash, TestMD5) {
    Hash::Ptr hash = createHash(Hash::MD5);
    ASSERT_TRUE(hash->digest(Buffer::BASE64)->equals(
        String::create("1B2M2Y8AsgTpgAmY7PhCfg==")));
    ASSERT_TRUE(hash->digest(Buffer::HEX)->equals(
        String::create("d41d8cd98f00b204e9800998ecf8427e")));

    hash = createHash(Hash::MD5);
    ASSERT_TRUE(hash->update(String::create("abc")));
    ASSERT_TRUE(hash->digest(Buffer::BASE64)->equals(
        String::create("kAFQmDzST7DWlj99KOF/cg==")));
    ASSERT_TRUE(hash->digest(Buffer::HEX)->equals(
        String::create("900150983cd24fb0d6963f7d28e17f72")));
    ASSERT_FALSE(hash->update(String::create("123")));
}

TEST(GTestCryptoHash, TestSHA1) {
    Hash::Ptr hash = createHash(Hash::SHA1);
    ASSERT_TRUE(hash->digest(Buffer::BASE64)->equals(
        String::create("2jmj7l5rSw0yVb/vlWAYkK/YBwk=")));
    ASSERT_TRUE(hash->digest(Buffer::HEX)->equals(
        String::create("da39a3ee5e6b4b0d3255bfef95601890afd80709")));

    hash = createHash(Hash::SHA1);
    ASSERT_TRUE(hash->update(String::create("abc")));
    ASSERT_TRUE(hash->digest(Buffer::BASE64)->equals(
        String::create("qZk+NkcGgWq6PiVxeFDCbJzQ2J0=")));
    ASSERT_TRUE(hash->digest(Buffer::HEX)->equals(
        String::create("a9993e364706816aba3e25717850c26c9cd0d89d")));
    ASSERT_FALSE(hash->update(String::create("123")));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
