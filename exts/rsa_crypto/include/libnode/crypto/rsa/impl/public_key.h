// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_CRYPTO_RSA_IMPL_PUBLIC_KEY_H_
#define LIBNODE_CRYPTO_RSA_IMPL_PUBLIC_KEY_H_

#define LIBNODE_CRYPTO_RSA_PUBLIC_KEY_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::crypto::rsa::PublicKey>::id() \
        || LIBJ_JS_OBJECT_INSTANCEOF(ID))

#endif  // LIBNODE_CRYPTO_RSA_IMPL_PUBLIC_KEY_H_
