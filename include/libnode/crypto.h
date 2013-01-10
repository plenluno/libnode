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

Cipher::Ptr createCipher(Cipher::Algorithm algo, Buffer::CPtr password);

Decipher::Ptr createDecipher(Cipher::Algorithm algo, Buffer::CPtr password);

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_H_
