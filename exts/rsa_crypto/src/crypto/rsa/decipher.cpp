// Copyright (c) 2013-2014 Plenluno All rights reserved.

#include <libnode/crypto/rsa/decipher.h>
#include <libnode/crypto/rsa/detail/decipher.h>

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
