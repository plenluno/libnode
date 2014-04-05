// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_KEY_PAIR_H_
#define LIBNODE_CRYPTO_RSA_KEY_PAIR_H_

#include <libnode/crypto/rsa/public_key.h>
#include <libnode/crypto/rsa/private_key.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

class KeyPair : LIBJ_JS_OBJECT(KeyPair)
 public:
    static CPtr create(Size modulusSize);

    virtual PublicKey::CPtr getPublicKey() const = 0;

    virtual PrivateKey::CPtr getPrivateKey() const = 0;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj

#include <libnode/crypto/rsa/impl/key_pair.h>

#endif  // LIBNODE_CRYPTO_RSA_KEY_PAIR_H_
