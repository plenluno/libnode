// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_PRIVATE_KEY_H_
#define LIBNODE_CRYPTO_RSA_PRIVATE_KEY_H_

#include <libj/js_object.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

class PrivateKey : LIBJ_JS_OBJECT(PrivateKey)
};

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj

#include <libnode/impl/crypto/rsa/private_key.h>

#endif  // LIBNODE_CRYPTO_RSA_PRIVATE_KEY_H_
