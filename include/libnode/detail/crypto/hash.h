// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_CRYPTO_HASH_H_
#define LIBNODE_DETAIL_CRYPTO_HASH_H_

#include <libnode/buffer.h>

#include <libj/detail/js_object.h>

#include <assert.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace libj {
namespace node {
namespace detail {
namespace crypto {

template<typename I>
class Hash : public libj::detail::JsObject<I> {
 public:
    typedef typename I::Algorithm Algorithm;

    Hash(Algorithm algo)
        : algorithm_(algo)
        , digest_(Buffer::null())
        , context_(0)
        , md_(0) {
        switch (algo) {
        case I::MD5:
            context_ = new MD5_CTX;
            md_ = new unsigned char[MD5_DIGEST_LENGTH];
            MD5_Init(static_cast<MD5_CTX*>(context_));
            break;
        case I::SHA1:
            context_ = new SHA_CTX;
            md_ = new unsigned char[SHA_DIGEST_LENGTH];
            SHA1_Init(static_cast<SHA_CTX*>(context_));
            break;
        case I::SHA256:
            context_ = new SHA256_CTX;
            md_ = new unsigned char[SHA256_DIGEST_LENGTH];
            SHA256_Init(static_cast<SHA256_CTX*>(context_));
            break;
        case I::SHA512:
            context_ = new SHA512_CTX;
            md_ = new unsigned char[SHA512_DIGEST_LENGTH];
            SHA512_Init(static_cast<SHA512_CTX*>(context_));
            break;
        default:
            assert(false);
        }
    }

    virtual ~Hash() {
        switch (algorithm_) {
        case I::MD5:
            delete static_cast<MD5_CTX*>(context_);
            break;
        case I::SHA1:
            delete static_cast<SHA_CTX*>(context_);
            break;
        case I::SHA256:
            delete static_cast<SHA256_CTX*>(context_);
            break;
        case I::SHA512:
            delete static_cast<SHA512_CTX*>(context_);
            break;
        default:
            assert(false);
        }
        delete [] md_;
    }

    virtual Boolean update(Buffer::CPtr buf) {
        if (buf) {
            return update(buf->data(), buf->length());
        } else {
            return false;
        }
    }

    virtual Boolean update(String::CPtr str, Buffer::Encoding enc) {
        if (str) {
            Buffer::CPtr buf = Buffer::create(str, enc);
            return update(buf);
        } else {
            return false;
        }
    }

    virtual Buffer::CPtr digest() {
        if (digest_) return digest_;

        Size len = 0;
        switch (algorithm_) {
        case I::MD5:
            MD5_Final(md_, static_cast<MD5_CTX*>(context_));
            len = MD5_DIGEST_LENGTH;
            break;
        case I::SHA1:
            SHA1_Final(md_, static_cast<SHA_CTX*>(context_));
            len = SHA_DIGEST_LENGTH;
            break;
        case I::SHA256:
            SHA256_Final(md_, static_cast<SHA256_CTX*>(context_));
            len = SHA256_DIGEST_LENGTH;
            break;
        case I::SHA512:
            SHA512_Final(md_, static_cast<SHA512_CTX*>(context_));
            len = SHA512_DIGEST_LENGTH;
            break;
        default:
            assert(false);
        }
        digest_ = Buffer::create(md_, len);
        return digest_;
    }

    virtual String::CPtr digest(Buffer::Encoding enc) {
        return digest()->toString(enc);
    }

 private:
    Boolean update(const void* dat, Size len) {
        if (!digest_ && dat) {
            switch (algorithm_) {
            case I::MD5:
                MD5_Update(static_cast<MD5_CTX*>(context_), dat, len);
                break;
            case I::SHA1:
                SHA1_Update(static_cast<SHA_CTX*>(context_), dat, len);
                break;
            case I::SHA256:
                SHA256_Update(static_cast<SHA256_CTX*>(context_), dat, len);
                break;
            case I::SHA512:
                SHA512_Update(static_cast<SHA512_CTX*>(context_), dat, len);
                break;
            default:
                assert(false);
            }
            return true;
        } else {
            return false;
        }
    }

 private:
    Algorithm algorithm_;
    Buffer::Ptr digest_;
    void* context_;
    unsigned char* md_;
};

}  // namespace crypto
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_CRYPTO_HASH_H_
