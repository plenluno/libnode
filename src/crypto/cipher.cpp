// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto/cipher.h>
#include <libnode/detail/crypto/cipher.h>

namespace libj {
namespace node {
namespace crypto {

Cipher::Ptr Cipher::create(Algorithm algo, Buffer::CPtr password) {
    return Ptr(new detail::crypto::Cipher<Cipher>(algo, password));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
