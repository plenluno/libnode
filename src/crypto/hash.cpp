// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/crypto/hash.h>
#include <libnode/detail/crypto/hash.h>

namespace libj {
namespace node {
namespace crypto {

Hash::Ptr Hash::create(Algorithm algo) {
    return Ptr(new detail::crypto::Hash<Hash>(algo));
}

}  // namespace crypto
}  // namespace node
}  // namespace libj
