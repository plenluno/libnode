// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_HASH_H_
#define LIBNODE_CRYPTO_HASH_H_

#include <libj/js_object.h>
#include <libj/string.h>

#include "libnode/buffer.h"

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

    virtual Boolean update(Buffer::CPtr buf) = 0;
    virtual Boolean update(String::CPtr str, Buffer::Encoding enc) = 0;
    virtual Value digest() = 0;
};

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_HASH_H_
