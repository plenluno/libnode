// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto/decipher.h>
#include <libnode/detail/crypto/decipher.h>

namespace libj {
namespace node {
namespace crypto {

Decipher::Ptr Decipher::create(Cipher::Algorithm algo, Buffer::CPtr password) {
    return Ptr(new detail::crypto::Decipher<Decipher>(algo, password));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
