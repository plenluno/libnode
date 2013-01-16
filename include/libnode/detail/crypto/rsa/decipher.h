// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_RSA_DECIPHER_H_
#define LIBNODE_DETAIL_CRYPTO_RSA_DECIPHER_H_

#include <libnode/detail/crypto/rsa/cipher_base.h>

#include <libj/cast.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

template<typename I>
class Decipher : public CipherBase<I> {
 public:
    Decipher(
        node::crypto::rsa::PrivateKey::CPtr key,
        node::crypto::rsa::Cipher::Padding padding) {
        PrivateKey::CPtr priKey = LIBJ_STATIC_CPTR_CAST(PrivateKey)(key);
        this->rsa_ = priKey->getRSA();
        this->key_ = key;
        this->padding_ = padding;
        assert(this->rsa_ && this->key_);
    }

    virtual Buffer::CPtr final() {
        Buffer::CPtr data = this->concat();
        if (!data) return Buffer::null();

        int size = RSA_size(this->rsa_);
        unsigned char* buf = new unsigned char[size];
        size = RSA_private_decrypt(
            data->length(),
            static_cast<unsigned char*>(const_cast<void*>(data->data())),
            buf,
            this->rsa_,
            this->convert(this->padding_));

        Buffer::CPtr decrypted = Buffer::null();
        if (size >= 0) {
            decrypted = Buffer::create(buf, size);
        }
        delete[] buf;
        return decrypted;
    }
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_RSA_DECIPHER_H_
