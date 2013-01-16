// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_CIPHER_H_
#define LIBNODE_CRYPTO_RSA_CIPHER_H_

#include <libnode/buffer.h>
#include <libnode/crypto/rsa/public_key.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

class Cipher : LIBJ_JS_OBJECT(Cipher)
 public:
    enum Padding {
        NONE,
        PKCS1,
        PKCS1_OAEP,
    };

    static Ptr create(PublicKey::CPtr key, Padding padding);

    virtual Boolean update(Buffer::CPtr data) = 0;

    virtual Buffer::CPtr final() = 0;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_RSA_CIPHER_H_
