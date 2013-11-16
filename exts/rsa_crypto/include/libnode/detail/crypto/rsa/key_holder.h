// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_RSA_KEY_HOLDER_H_
#define LIBNODE_DETAIL_CRYPTO_RSA_KEY_HOLDER_H_

#include <libj/mutable.h>

#include <openssl/rsa.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {
namespace rsa {

class KeyHolder : public libj::Mutable {
 public:
    LIBJ_MUTABLE_DEFS(KeyHolder, libj::Mutable);

    KeyHolder() : rsa_(NULL) {}

    virtual ~KeyHolder() {
        if (rsa_) {
            RSA_free(rsa_);
        }
    }

    virtual String::CPtr toString() const {
        return String::null();
    }

    RSA* getRSA() const {
        return rsa_;
    }

    void setRSA(RSA* rsa) {
        rsa_ = rsa;
    }

 private:
    RSA* rsa_;
};

}  // namespace rsa
}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_RSA_KEY_HOLDER_H_
