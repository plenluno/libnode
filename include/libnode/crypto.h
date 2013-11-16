// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_H_
#define LIBNODE_CRYPTO_H_

#include <libnode/crypto/hash.h>
#include <libnode/crypto/cipher.h>
#include <libnode/crypto/decipher.h>

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr createHash(Hash::Algorithm algo);

Cipher::Ptr createCipher(
    Cipher::Algorithm algo, Buffer::CPtr password);

Cipher::Ptr createCipheriv(
    Cipher::Algorithm algo, Buffer::CPtr key, Buffer::CPtr iv);

Decipher::Ptr createDecipher(
    Cipher::Algorithm algo, Buffer::CPtr password);

Decipher::Ptr createDecipheriv(
    Cipher::Algorithm algo, Buffer::CPtr key, Buffer::CPtr iv);

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_H_
