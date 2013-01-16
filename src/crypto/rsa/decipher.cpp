// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/crypto/rsa/decipher.h>
#include <libnode/detail/crypto/rsa/decipher.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

Decipher::Ptr Decipher::create(
    PrivateKey::CPtr key,
    Cipher::Padding padding) {
    if (key) {
        return Ptr(new detail::crypto::rsa::Decipher<Decipher>(key, padding));
    } else {
        return null();
    }
}

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj
