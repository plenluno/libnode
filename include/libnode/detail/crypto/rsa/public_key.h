// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_RSA_PUBLIC_KEY_H_
#define LIBNODE_DETAIL_CRYPTO_RSA_PUBLIC_KEY_H_

#include <libnode/crypto/rsa/public_key.h>
#include <libnode/detail/crypto/rsa/key_holder.h>

#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

class PublicKey
    : public libj::detail::JsObject<node::crypto::rsa::PublicKey> {
 public:
    LIBJ_MUTABLE_DEFS(PublicKey, node::crypto::rsa::PublicKey);

    PublicKey(KeyHolder::Ptr holder) : holder_(holder) {}

    RSA* getRSA() const {
        return holder_->getRSA();
    }

 private:
    KeyHolder::Ptr holder_;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_RSA_PUBLIC_KEY_H_
