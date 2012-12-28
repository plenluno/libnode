// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/config.h>
#include <libnode/util.h>

#include <libj/error.h>
#include <libj/js_array.h>
#include <libj/js_regexp.h>

#include <assert.h>
#ifdef LIBNODE_USE_OPENSSL
    #include <openssl/bio.h>
    #include <openssl/buffer.h>
    #include <openssl/evp.h>
#else
    #include <b64/b64.h>
#endif

namespace libj {
namespace node {
namespace util {

Boolean isArray(const Value& val) {
    return val.instanceof(Type<JsArray>::id());
}

Boolean isError(const Value& val) {
    return val.instanceof(Type<libj::Error>::id());
}

Boolean isRegExp(const Value& val) {
    return val.instanceof(Type<JsRegExp>::id());
}

Boolean extend(JsObject::Ptr extended, JsObject::CPtr original) {
    if (!extended) return false;

    Set::CPtr keys = original->keySet();
    Iterator::Ptr itr = keys->iterator();
    while (itr->hasNext()) {
        String::CPtr key = toCPtr<String>(itr->next());
        extended->put(key, original->get(key));
    }
    return true;
}

// -- hexEncode & hexDecode --

String::CPtr hexEncode(Buffer::CPtr buf) {
    if (buf) {
        if (buf->isEmpty()) {
            return String::create();
        } else {
            return hexEncode(buf->data(), buf->length());
        }
    } else {
        return String::null();
    }
}

String::CPtr hexEncode(const void* data, Size len) {
    if (!data) return String::null();

    const UByte* bytes = static_cast<const UByte*>(data);
    Buffer::Ptr encoded = Buffer::create(len * 2);
    for (Size i = 0; i < len; i++) {
        UByte byte = bytes[i];
        const UByte msb = (byte >> 4) & 0x0f;
        const UByte lsb = byte & 0x0f;
        encoded->writeUInt8(
            (msb < 10) ? msb + '0' : (msb - 10) + 'a', i * 2);
        encoded->writeUInt8(
            (lsb < 10) ? lsb + '0' : (lsb - 10) + 'a', i * 2 + 1);
    }
    return encoded->toString();
}

static Boolean decodeHex(Char c, UByte* decoded) {
    if (!decoded) return false;

    if (c >= '0' && c <= '9') {
        *decoded = c - '0';
        return true;
    } else if (c >= 'a' && c <= 'f') {
        *decoded = c - 'a' + 10;
        return true;
    } else {
        return false;
    }
}

Buffer::Ptr hexDecode(String::CPtr str) {
    if (!str) return Buffer::null();
    if (str->length() & 1) return Buffer::null();

    Size len = str->length() >> 1;
    Buffer::Ptr decoded = Buffer::create(len);
    for (Size i = 0; i < len; i++) {
        Char c1 = str->charAt(i * 2);
        Char c2 = str->charAt(i * 2 + 1);
        UByte msb, lsb;
        if (decodeHex(c1, &msb) && decodeHex(c2, &lsb)) {
            decoded->writeUInt8((msb << 4) + lsb, i);
        } else {
            return Buffer::null();
        }
    }
    return decoded;
}


// -- base64Encode & base64Decode --

String::CPtr base64Encode(Buffer::CPtr buf) {
    if (buf) {
        if (buf->isEmpty()) {
            return String::create();
        } else {
            return base64Encode(buf->data(), buf->length());
        }
    } else {
        return String::null();
    }
}

#ifdef LIBNODE_USE_OPENSSL

String::CPtr base64Encode(const void* data, Size len) {
    if (!data) return String::null();
    if (!len) return String::create();

    BIO* bio = BIO_new(BIO_f_base64());
    BIO* bioMem = BIO_new(BIO_s_mem());
    bio = BIO_push(bio, bioMem);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data, len);
    int ret = BIO_flush(bio);
    assert(ret == 1);

    BUF_MEM* bufMem;
    BIO_get_mem_ptr(bio, &bufMem);
    String::CPtr encoded =
        String::create(bufMem->data, String::UTF8, bufMem->length);
    BIO_free_all(bio);
    return encoded;
}

Buffer::Ptr base64Decode(String::CPtr str) {
    if (!str) return Buffer::null();
    if (!str->length()) return Buffer::create();

    std::string src = str->toStdString();
    Size len = src.length();
    if (len != str->length()) return Buffer::null();

    BIO* bio = BIO_new(BIO_f_base64());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO* bioMem = BIO_new_mem_buf(const_cast<char*>(src.c_str()), len);
    bio = BIO_push(bio, bioMem);

    char* dst = new char[len + 1];
    int readLen = BIO_read(bio, dst, len);
    Buffer::Ptr decoded;
    if (readLen > 0) {
        decoded = Buffer::create(dst, readLen);
    } else {
        decoded = Buffer::null();
    }
    BIO_free_all(bio);
    delete[] dst;
    return decoded;
}

#else  // LIBNODE_USE_OPENSSL

String::CPtr base64Encode(const void* data, Size len) {
    if (!data) return String::null();
    if (!len) return String::create();

    Size size = b64::b64_encode(data, len, NULL, 0);
    char* buf = new char[size];
    size = b64::b64_encode(data, len, buf, size);
    String::CPtr encoded = String::create(buf, String::UTF8, size);
    delete[] buf;
    return encoded;
}

Buffer::Ptr base64Decode(String::CPtr str) {
    if (!str) return Buffer::null();
    if (!str->length()) return Buffer::create();

    std::string src = str->toStdString();
    Size len = src.length();
    b64::B64_RC rc;
    const char* bad = NULL;
    const unsigned flags = b64::B64_F_STOP_ON_UNKNOWN_CHAR;
    Size size = b64::b64_decode2(src.c_str(), len, NULL, 0, flags, &bad, &rc);

    char* dst = new char[size];
    size = b64::b64_decode2(src.c_str(), len, dst, size, flags, &bad, &rc);
    Buffer::Ptr decoded;
    if (size && rc == b64::B64_RC_OK && !bad) {
        decoded = Buffer::create(dst, size);
    } else {
        decoded = Buffer::null();
    }
    delete[] dst;
    return decoded;
}

#endif  // LIBNODE_USE_OPENSSL

// -- percentEncode & percentDecode --

static UInt valueFromHexChar(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        return 0;
    }
}

static char hexCharFromValue(UInt value) {
    if (value >= 16) {
        return '0';
    } else {
        return "0123456789ABCDEF"[value];
    }
}

static Size percentEncode(
    char* encoded,
    Size encodedLength,
    const char* source,
    Size sourceLength) {
    if (!encoded || !source) return 0;

    const char* encodedStart = encoded;
    const char* sourceEnd = source + sourceLength;
    char temp;
    encodedLength--;
    while (source < sourceEnd && encodedLength) {
        temp = *source;
        if ((temp >= 'A' && temp <= 'Z') ||
            (temp >= 'a' && temp <= 'z') ||
            (temp >= '0' && temp <= '9') ||
            temp == '-' ||
            temp == '.' ||
            temp == '_' ||
            temp == '~') {
            *(encoded++) = temp;
        } else {
            *(encoded++) = '%';
            if (!(--encodedLength)) break;
            *(encoded++) = hexCharFromValue((unsigned char)temp >> 4);
            if (!(--encodedLength)) break;
            *(encoded++) = hexCharFromValue(temp & 0x0F);
            encodedLength -= 2;
        }
        source++;
        encodedLength--;
    }
    *encoded = '\0';
    return encoded - encodedStart;
}

static Size percentDecode(
    char* decoded,
    Size decodedLength,
    const char* source) {
    if (!decoded || !source) return 0;

    const char* start = decoded;
    decodedLength--;
    while (*source && decodedLength) {
        if (*source == '%') {
            if (*(source + 1) == '\0') break;
            *(decoded++) = static_cast<char>(
                (valueFromHexChar(*(source + 1)) << 4) +
                 valueFromHexChar(*(source + 2)));
            source += 3;
        } else if (*source == '+') {
            *(decoded++) = ' ';
            source++;
        } else {
            *(decoded++) = *(source++);
        }
        decodedLength--;
    }
    *decoded = '\0';
    return decoded - start;
}

String::CPtr percentEncode(String::CPtr str, String::Encoding enc) {
    if (!str || str->length() == 0)
        return String::create();

    Buffer::Ptr buf;
    switch (enc) {
    case String::UTF8:
        buf = Buffer::create(str, Buffer::UTF8);
        break;
    case String::UTF16BE:
        buf = Buffer::create(str, Buffer::UTF16BE);
        break;
    case String::UTF16LE:
        buf = Buffer::create(str, Buffer::UTF16LE);
        break;
    case String::UTF32BE:
        buf = Buffer::create(str, Buffer::UTF32BE);
        break;
    case String::UTF32LE:
        buf = Buffer::create(str, Buffer::UTF32LE);
        break;
    }
    Size sourceLen = buf->length();
    const char* source = static_cast<const char*>(buf->data());
    Size encodedLen = sourceLen * 3 + 1;
    char* encoded = new char[encodedLen];
    percentEncode(encoded, encodedLen, source, sourceLen);
    String::CPtr res = String::create(encoded);
    delete[] encoded;
    return res;
}

String::CPtr percentDecode(String::CPtr str, String::Encoding enc) {
    if (!str || str->isEmpty())
        return String::create();

    Size len = str->length() + 1;
    char* decoded = new char[len];
    Size size = percentDecode(decoded, len, str->toStdString().c_str());
    String::CPtr res = String::create(decoded, enc, size);
    delete[] decoded;
    return res;
}

}  // namespace util
}  // namespace node
}  // namespace libj
