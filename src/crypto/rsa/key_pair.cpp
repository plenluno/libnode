// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/crypto/rsa/key_pair.h>
#include <libnode/detail/crypto/rsa/key_pair.h>

namespace libj {
namespace node {
namespace crypto {
namespace rsa {

KeyPair::CPtr KeyPair::create(Size modulusSize) {
    if (modulusSize) {
        detail::crypto::rsa::KeyPair::Ptr keyPair(
            new detail::crypto::rsa::KeyPair(modulusSize));
        return keyPair->getRSA() ? keyPair : null();
    } else {
        return null();
    }
}

}  // namespace rsa
}  // namespace crypto
}  // namespace node
}  // namespace libj
