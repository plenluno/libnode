// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_CIPHER_BASE_H_
#define LIBNODE_DETAIL_CRYPTO_CIPHER_BASE_H_

#include <libnode/buffer.h>
#include <libnode/crypto/cipher.h>

#include <libj/detail/js_object.h>

#include <assert.h>
#include <openssl/evp.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {

template<typename I>
class CipherBase : public libj::detail::JsObject<I> {
 public:
    typedef node::crypto::Cipher::Algorithm Algorithm;

    CipherBase()
        : cipher_(NULL)
        , initialized_(false) {}

    virtual ~CipherBase() {
        if (initialized_) {
            EVP_CIPHER_CTX_cleanup(&ctx_);
        }
    }

    virtual Buffer::CPtr update(Buffer::CPtr data) {
        if (!initialized_) return Buffer::null();

        Size len = data->length();
        int bufLen = EVP_CIPHER_CTX_block_size(&ctx_) + len;
        unsigned char* buf = new unsigned char[bufLen];

        EVP_CipherUpdate(
            &ctx_,
            buf,
            &bufLen,
            static_cast<unsigned char*>(const_cast<void*>(data->data())),
            len);

        Buffer::Ptr res = Buffer::create(buf, bufLen);
        delete[] buf;
        return res;
    }

    virtual Buffer::CPtr final() {
        if (!initialized_) return Buffer::null();

        int bufLen = EVP_CIPHER_CTX_block_size(&ctx_);
        unsigned char* buf = new unsigned char[bufLen];
        int r = EVP_CipherFinal_ex(&ctx_, buf, &bufLen);

        Buffer::Ptr res;
        if (bufLen == 0 || r == 0) {
            res = Buffer::null();
        } else {
            res = Buffer::create(buf, bufLen);
        }

        delete[] buf;
        EVP_CIPHER_CTX_cleanup(&ctx_);
        initialized_ = false;
        return res;
    }

    virtual void setAutoPadding(Boolean autoPadding) {
        if (initialized_) {
            EVP_CIPHER_CTX_set_padding(&ctx_, autoPadding ? 1 : 0);
        }
    }

 protected:
    static void initializeCipher(
        Algorithm algo,
        Buffer::CPtr passwd,
        Boolean encrypt,
        EVP_CIPHER_CTX* ctx,
        const EVP_CIPHER** cipher,
        Boolean* initialized) {
        static Boolean first = true;
        if (first) {
            OpenSSL_add_all_ciphers();
            first = false;
        }

        switch (algo) {
        case node::crypto::Cipher::AES_128_CBC:
            *cipher = EVP_get_cipherbyname("aes-128-cbc");
            break;
        case node::crypto::Cipher::AES_128_ECB:
            *cipher = EVP_get_cipherbyname("aes-128-ecb");
            break;
        case node::crypto::Cipher::AES_192_CBC:
            *cipher = EVP_get_cipherbyname("aes-192-cbc");
            break;
        case node::crypto::Cipher::AES_192_ECB:
            *cipher = EVP_get_cipherbyname("aes-192-ecb");
            break;
        case node::crypto::Cipher::AES_256_CBC:
            *cipher = EVP_get_cipherbyname("aes-256-cbc");
            break;
        case node::crypto::Cipher::AES_256_ECB:
            *cipher = EVP_get_cipherbyname("aes-256-ecb");
            break;
        default:
            assert(false);
        }
        assert(!!*cipher);

        unsigned char key[EVP_MAX_KEY_LENGTH];
        unsigned char iv[EVP_MAX_IV_LENGTH];
        int keyLen = EVP_BytesToKey(
            *cipher,
            EVP_md5(),
            NULL,
            static_cast<unsigned char*>(const_cast<void*>(passwd->data())),
            passwd->length(),
            1,
            key,
            iv);

        EVP_CIPHER_CTX_init(ctx);
        EVP_CipherInit_ex(ctx, *cipher, NULL, NULL, NULL, encrypt);

        if (!EVP_CIPHER_CTX_set_key_length(ctx, keyLen)) {
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }

        EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, encrypt);
        *initialized = true;
    }

    EVP_CIPHER_CTX ctx_;
    const EVP_CIPHER* cipher_;
    Boolean initialized_;
};

}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_CIPHER_BASE_H_
