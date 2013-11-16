// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_DECIPHER_H_
#define LIBNODE_CRYPTO_RSA_DECIPHER_H_

#include <libnode/crypto/rsa/cipher.h>
#include <libnode/crypto/rsa/private_key.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

class Decipher : LIBJ_JS_OBJECT(Decipher)
 public:
    static Ptr create(PrivateKey::CPtr key, Cipher::Padding padding);

    virtual Boolean update(Buffer::CPtr data) = 0;

    virtual Buffer::CPtr final() = 0;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_RSA_DECIPHER_H_
