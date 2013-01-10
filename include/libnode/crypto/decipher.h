// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_DECIPHER_H_
#define LIBNODE_CRYPTO_DECIPHER_H_

#include <libnode/crypto/cipher.h>

namespace libj {
namespace node {
namespace crypto {

class Decipher : LIBJ_JS_OBJECT(Decipher)
 public:
    static Ptr create(Cipher::Algorithm algo, Buffer::CPtr password);

    virtual Buffer::CPtr update(Buffer::CPtr buf) = 0;

    virtual Buffer::CPtr final() = 0;

    virtual void setAutoPadding(Boolean autoPadding = true) = 0;
};

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_DECIPHER_H_
