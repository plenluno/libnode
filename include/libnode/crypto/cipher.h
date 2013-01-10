// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_CIPHER_H_
#define LIBNODE_CRYPTO_CIPHER_H_

#include <libnode/buffer.h>

#include <libj/js_object.h>

namespace libj {
namespace node {
namespace crypto {

class Cipher : LIBJ_JS_OBJECT(Cipher)
 public:
    enum Algorithm {
        AES_128_CBC,
        AES_128_ECB,
        AES_192_CBC,
        AES_192_ECB,
        AES_256_CBC,
        AES_256_ECB,
    };

    static Ptr create(Algorithm algo, Buffer::CPtr password);

    virtual Buffer::CPtr update(Buffer::CPtr data) = 0;

    virtual Buffer::CPtr final() = 0;

    virtual void setAutoPadding(Boolean autoPadding = true) = 0;
};

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_CIPHER_H_
