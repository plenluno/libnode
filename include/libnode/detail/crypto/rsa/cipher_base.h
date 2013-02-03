// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_BASE_H_
#define LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_BASE_H_

#include <libj/js_array.h>
#include <libnode/detail/crypto/rsa/key_pair.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

template<typename I>
class CipherBase : public libj::detail::JsObject<I> {
 public:
    typedef node::crypto::rsa::Cipher::Padding Padding;

    CipherBase()
        : rsa_(NULL)
        , key_(JsObject::null())
        , padding_(node::crypto::rsa::Cipher::NONE)
        , bufs_(JsArray::create()) {}

    virtual Boolean update(Buffer::CPtr data) {
        if (data && bufs_) {
            bufs_->add(data);
            return true;
        } else {
            return false;
        }
    }

 protected:
    static int convert(Padding padding) {
        switch (padding) {
        case node::crypto::rsa::Cipher::NONE:
            return RSA_NO_PADDING;
        case node::crypto::rsa::Cipher::PKCS1:
            return RSA_PKCS1_PADDING;
        case node::crypto::rsa::Cipher::PKCS1_OAEP:
            return RSA_PKCS1_OAEP_PADDING;
        default:
            return RSA_NO_PADDING;
        }
    }

    Buffer::CPtr concat() {
        Buffer::CPtr buf = Buffer::concat(bufs_);
        bufs_ = JsArray::null();
        return buf;
    }

 protected:
    RSA* rsa_;
    libj::JsObject::CPtr key_;
    Padding padding_;
    JsArray::Ptr bufs_;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_RSA_CIPHER_BASE_H_
