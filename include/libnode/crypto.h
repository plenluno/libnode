// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_H_
#define LIBNODE_CRYPTO_H_

#include "libnode/crypto/hash.h"

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr createHash(Hash::Algorithm algo);

}  // namespace crypto
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_CRYPTO_H_
