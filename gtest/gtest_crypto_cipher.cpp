// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/crypto.h>

namespace libj {
namespace node {
namespace crypto {

TEST(GTestCryptoCipher, TestCreateCipher) {
    Buffer::Ptr passwd = Buffer::create("123", 3);
    ASSERT_TRUE(!!createCipher(Cipher::AES_128_CBC, passwd));
    ASSERT_TRUE(!!createCipher(Cipher::AES_128_ECB, passwd));
    ASSERT_TRUE(!!createCipher(Cipher::AES_192_CBC, passwd));
    ASSERT_TRUE(!!createCipher(Cipher::AES_192_ECB, passwd));
    ASSERT_TRUE(!!createCipher(Cipher::AES_256_CBC, passwd));
    ASSERT_TRUE(!!createCipher(Cipher::AES_256_ECB, passwd));
}

TEST(GTestCryptoCipher, TestCreateDecipher) {
    Buffer::Ptr passwd = Buffer::create("123", 3);
    ASSERT_TRUE(!!createDecipher(Cipher::AES_128_CBC, passwd));
    ASSERT_TRUE(!!createDecipher(Cipher::AES_128_ECB, passwd));
    ASSERT_TRUE(!!createDecipher(Cipher::AES_192_CBC, passwd));
    ASSERT_TRUE(!!createDecipher(Cipher::AES_192_ECB, passwd));
    ASSERT_TRUE(!!createDecipher(Cipher::AES_256_CBC, passwd));
    ASSERT_TRUE(!!createDecipher(Cipher::AES_256_ECB, passwd));
}

TEST(GTestCryptoCipher, TestCBC) {
    Buffer::Ptr passwd = Buffer::create("abc", 3);
    Buffer::Ptr data = Buffer::create("abcdefghijklmnopqrstuvwxyz", 26);

    Cipher::Ptr cipher = createCipher(Cipher::AES_128_CBC, passwd);
    Buffer::CPtr buf1 = cipher->update(data);
    Buffer::CPtr buf2 = cipher->final();

    Decipher::Ptr decipher = createDecipher(Cipher::AES_128_CBC, passwd);
    JsArray::Ptr bufs = JsArray::create();
    bufs->add(decipher->update(buf1));
    bufs->add(decipher->update(buf2));
    bufs->add(decipher->final());

    ASSERT_TRUE(data->toString()->equals(Buffer::concat(bufs)->toString()));
}

TEST(GTestCryptoCipher, TestECB) {
    Buffer::Ptr passwd = Buffer::create("abc", 3);
    Buffer::Ptr data = Buffer::create("abcdefghijklmnopqrstuvwxyz", 26);

    Cipher::Ptr cipher = createCipher(Cipher::AES_128_ECB, passwd);
    Buffer::CPtr buf1 = cipher->update(data);
    Buffer::CPtr buf2 = cipher->final();

    Decipher::Ptr decipher = createDecipher(Cipher::AES_128_ECB, passwd);
    JsArray::Ptr bufs = JsArray::create();
    bufs->add(decipher->update(buf1));
    bufs->add(decipher->update(buf2));
    bufs->add(decipher->final());

    ASSERT_TRUE(data->toString()->equals(Buffer::concat(bufs)->toString()));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
