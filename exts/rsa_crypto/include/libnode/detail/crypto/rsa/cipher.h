// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_H_
#define LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_H_

#include <libnode/detail/crypto/rsa/cipher_base.h>

#include <libj/cast.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

template<typename I>
class Cipher : public CipherBase<I> {
 public:
    Cipher(
        node::crypto::rsa::PublicKey::CPtr key,
        node::crypto::rsa::Cipher::Padding padding) {
        PublicKey::CPtr pubKey = LIBJ_STATIC_CPTR_CAST(PublicKey)(key);
        this->rsa_ = pubKey->getRSA();
        this->key_ = key;
        this->padding_ = padding;
        assert(this->rsa_ && this->key_);
    }

    virtual Buffer::CPtr final() {
        Buffer::CPtr data = this->concat();
        if (!data) return Buffer::null();

        int size = RSA_size(this->rsa_);
        unsigned char* buf = new unsigned char[size];
        size = RSA_public_encrypt(
            data->length(),
            static_cast<unsigned char*>(const_cast<void*>(data->data())),
            buf,
            this->rsa_,
            this->convert(this->padding_));

        Buffer::CPtr encrypted = Buffer::null();
        if (size >= 0) {
            encrypted = Buffer::create(buf, size);
        }
        delete[] buf;
        return encrypted;
    }
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_H_
