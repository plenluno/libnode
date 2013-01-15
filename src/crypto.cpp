// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto.h>

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr createHash(Hash::Algorithm algo) {
    return Hash::create(algo);
}

Cipher::Ptr createCipher(
    Cipher::Algorithm algo, Buffer::CPtr passwd) {
    return Cipher::create(algo, passwd);
}

Cipher::Ptr createCipheriv(
    Cipher::Algorithm algo, Buffer::CPtr key, Buffer::CPtr iv) {
    return Cipher::create(algo, key, iv);
}

Decipher::Ptr createDecipher(
    Cipher::Algorithm algo, Buffer::CPtr passwd) {
    return Decipher::create(algo, passwd);
}

Decipher::Ptr createDecipheriv(
    Cipher::Algorithm algo, Buffer::CPtr key, Buffer::CPtr iv) {
    return Decipher::create(algo, key, iv);
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
