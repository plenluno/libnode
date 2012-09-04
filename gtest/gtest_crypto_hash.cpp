// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/crypto.h>

namespace libj {
namespace node {
namespace crypto {

TEST(GTestCryptoHash, TestCreateHash) {
    Hash::Ptr hash = createHash(Hash::MD5);
    ASSERT_TRUE(hash);
}

TEST(GTestCryptoHash, TestMD5) {
    Hash::Ptr hash = createHash(Hash::MD5);
    ASSERT_TRUE(hash->update(String::create("abc")));
    ASSERT_TRUE(hash->digest(Buffer::BASE64)->equals(
        String::create("kAFQmDzST7DWlj99KOF/cg==")));
    ASSERT_TRUE(hash->digest(Buffer::HEX)->equals(
        String::create("900150983cd24fb0d6963f7d28e17f72")));
    ASSERT_FALSE(hash->update(String::create("123")));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
