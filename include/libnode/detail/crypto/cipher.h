// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_CIPHER_H_
#define LIBNODE_DETAIL_CRYPTO_CIPHER_H_

#include <libnode/detail/crypto/cipher_base.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {

template<typename I>
class Cipher : public CipherBase<I> {
 public:
    typedef typename CipherBase<I>::Algorithm Algorithm;

    Cipher(Algorithm algo, Buffer::CPtr passwd) {
        this->initializeCipher(
            algo,
            passwd,
            Buffer::null(),
            Buffer::null(),
            true,
            &this->ctx_,
            &this->cipher_,
            &this->initialized_);
    }

    Cipher(Algorithm algo, Buffer::CPtr key, Buffer::CPtr iv) {
        this->initializeCipher(
            algo,
            Buffer::null(),
            key,
            iv,
            true,
            &this->ctx_,
            &this->cipher_,
            &this->initialized_);
    }
};

}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_CIPHER_H_
