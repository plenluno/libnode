// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto/cipher.h>
#include <libnode/detail/crypto/cipher.h>

namespace libj {
namespace node {
namespace crypto {

Cipher::Ptr Cipher::create(
    Algorithm algo,
    Buffer::CPtr passwd) {
    detail::crypto::Cipher<Cipher>* cipher =
        new detail::crypto::Cipher<Cipher>(algo, passwd);
    if (cipher->isInitialized()) {
        return Ptr(cipher);
    } else {
        delete cipher;
        return null();
    }
}

Cipher::Ptr Cipher::create(
    Algorithm algo,
    Buffer::CPtr key,
    Buffer::CPtr iv) {
    detail::crypto::Cipher<Cipher>* cipher =
        new detail::crypto::Cipher<Cipher>(algo, key, iv);
    if (cipher->isInitialized()) {
        return Ptr(cipher);
    } else {
        delete cipher;
        return null();
    }
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
