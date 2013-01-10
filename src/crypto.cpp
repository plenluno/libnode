// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto.h>

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr createHash(Hash::Algorithm algo) {
    return Hash::create(algo);
}

Cipher::Ptr createCipher(Cipher::Algorithm algo, Buffer::CPtr passwd) {
    return Cipher::create(algo, passwd);
}

Decipher::Ptr createDecipher(Cipher::Algorithm algo, Buffer::CPtr passwd) {
    return Decipher::create(algo, passwd);
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
