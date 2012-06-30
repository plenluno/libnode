// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_HASH_H_
#define LIBNODE_CRYPTO_HASH_H_

#include <libj/js_object.h>
#include <libj/string.h>

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

    static Ptr create(Algorithm alg);

    virtual void update(const Value& data);
    virtual Value digest() = 0;
};

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_HASH_H_
