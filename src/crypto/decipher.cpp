// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto/decipher.h>
#include <libnode/detail/crypto/decipher.h>

namespace libj {
namespace node {
namespace crypto {

Decipher::Ptr Decipher::create(
    Cipher::Algorithm algo,
    Buffer::CPtr passwd) {
    detail::crypto::Decipher<Decipher>* decipher =
        new detail::crypto::Decipher<Decipher>(algo, passwd);
    if (decipher->isInitialized()) {
        return Ptr(decipher);
    } else {
        delete decipher;
        return null();
    }
}

Decipher::Ptr Decipher::create(
    Cipher::Algorithm algo,
    Buffer::CPtr key,
    Buffer::CPtr iv) {
    detail::crypto::Decipher<Decipher>* decipher =
        new detail::crypto::Decipher<Decipher>(algo, key, iv);
    if (decipher->isInitialized()) {
        return Ptr(decipher);
    } else {
        delete decipher;
        return null();
    }
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
