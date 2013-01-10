// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_HASH_H_
#define LIBNODE_CRYPTO_HASH_H_

#include <libnode/buffer.h>

#include <libj/js_object.h>

namespace libj {
namespace node {
namespace crypto {

class Hash : LIBJ_JS_OBJECT(Hash)
 public:
    enum Algorithm {
        MD5,
        SHA1,
        SHA256,
        SHA512,
    };

    static Ptr create(Algorithm algo);

    virtual Boolean update(Buffer::CPtr data) = 0;

    virtual Buffer::CPtr digest() = 0;
};

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_HASH_H_
