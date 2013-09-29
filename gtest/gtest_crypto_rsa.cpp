// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/crypto/rsa.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

TEST(GTestCryptoRsa, TestCreateKeyPair) {
    ASSERT_TRUE(!KeyPair::create(0));
    ASSERT_TRUE(!!KeyPair::create(1));
    ASSERT_TRUE(!!KeyPair::create(1024));
    ASSERT_TRUE(!!KeyPair::create(2048));
}

TEST(GTestCryptoRsa, TestCreateCipher) {
    KeyPair::CPtr keyPair = KeyPair::create(512);
    PublicKey::CPtr pubKey = keyPair->getPublicKey();
    ASSERT_TRUE(!!Cipher::create(pubKey, Cipher::NONE));
    ASSERT_TRUE(!!Cipher::create(pubKey, Cipher::PKCS1));
    ASSERT_TRUE(!!Cipher::create(pubKey, Cipher::PKCS1_OAEP));
    ASSERT_TRUE(!Cipher::create(PublicKey::null(), Cipher::NONE));
}

TEST(GTestCryptoRsa, TestCreateDecipher) {
    KeyPair::CPtr keyPair = KeyPair::create(512);
    PrivateKey::CPtr priKey = keyPair->getPrivateKey();
    ASSERT_TRUE(!!Decipher::create(priKey, Cipher::NONE));
    ASSERT_TRUE(!!Decipher::create(priKey, Cipher::PKCS1));
    ASSERT_TRUE(!!Decipher::create(priKey, Cipher::PKCS1_OAEP));
    ASSERT_TRUE(!Decipher::create(PrivateKey::null(), Cipher::NONE));
}

TEST(GTestCryptoRsa, TestUpdateAndFinal) {
    const Size modulusSize = 1024;
    KeyPair::CPtr keyPair = KeyPair::create(modulusSize);
    PublicKey::CPtr pubKey = keyPair->getPublicKey();
    PrivateKey::CPtr priKey = keyPair->getPrivateKey();
    Buffer::Ptr data = Buffer::create("abcdefghijklmnopqrstuvwxyz", 26);

    Cipher::Ptr cipher = Cipher::create(pubKey, Cipher::PKCS1_OAEP);
    ASSERT_TRUE(cipher->update(data));
    Buffer::CPtr encrypted = cipher->final();
    ASSERT_TRUE(encrypted && encrypted->length() * 8 == modulusSize);

    Decipher::Ptr decipher = Decipher::create(priKey, Cipher::PKCS1_OAEP);
    ASSERT_TRUE(decipher->update(encrypted));
    Buffer::CPtr decrypted = decipher->final();

    ASSERT_TRUE(data->toString()->equals(decrypted->toString()));
}

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj
