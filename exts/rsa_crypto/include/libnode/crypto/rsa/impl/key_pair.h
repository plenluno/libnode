// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_IMPL_KEY_PAIR_H_
#define LIBNODE_CRYPTO_RSA_IMPL_KEY_PAIR_H_

#define LIBNODE_CRYPTO_RSA_KEY_PAIR_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::crypto::rsa::KeyPair>::id() \
        || LIBJ_JS_OBJECT_INSTANCEOF(ID))

#endif  // LIBNODE_CRYPTO_RSA_IMPL_KEY_PAIR_H_
