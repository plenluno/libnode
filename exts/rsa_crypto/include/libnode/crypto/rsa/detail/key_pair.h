// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_DETAIL_KEY_PAIR_H_
#define LIBNODE_CRYPTO_RSA_DETAIL_KEY_PAIR_H_

#include <libnode/crypto/rsa/key_pair.h>
#include <libnode/crypto/rsa/detail/public_key.h>
#include <libnode/crypto/rsa/detail/private_key.h>

#include <assert.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

class KeyPair
    : public libj::detail::JsObject<node::crypto::rsa::KeyPair> {
 public:
    LIBJ_MUTABLE_DEFS(KeyPair, LIBNODE_CRYPTO_RSA_KEY_PAIR);

    KeyPair(Size modulusSize) : holder_(new KeyHolder()) {
        assert(modulusSize);
        RSA* rsa = RSA_generate_key(modulusSize, RSA_F4, NULL, NULL);
        if (RSA_check_key(rsa)) {
            holder_->setRSA(rsa);
        }
    }

    RSA* getRSA() const {
        return holder_->getRSA();
    }

    virtual node::crypto::rsa::PublicKey::CPtr getPublicKey() const {
        if (getRSA()) {
            return PublicKey::Ptr(new PublicKey(holder_));
        } else {
            return PublicKey::null();
        }
    }

    virtual node::crypto::rsa::PrivateKey::CPtr getPrivateKey() const {
        if (getRSA()) {
            return PrivateKey::Ptr(new PrivateKey(holder_));
        } else {
            return PrivateKey::null();
        }
    }

 private:
    KeyHolder::Ptr holder_;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_RSA_DETAIL_KEY_PAIR_H_
