// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/crypto.h"

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr createHash(Hash::Algorithm algo) {
    return Hash::create(algo);
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
