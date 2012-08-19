// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/error.h>
#include <libj/js_array.h>
#include <libj/js_regexp.h> 
#include <libnode/buffer.h>

#include "libnode/util.h"

namespace libj {
namespace node {
namespace util {

Boolean isArray(const Value& val) {
    return val.instanceof(Type<JsArray>::id());
}

Boolean isError(const Value& val) {
    return val.instanceof(Type<Error>::id());
}

Boolean isRegExp(const Value& val) {
    return val.instanceof(Type<JsRegExp>::id());
}

#define ASCII_ALPHA1_START  (0x41)  // A
#define ASCII_ALPHA1_END    (0x5A)  // Z
#define ASCII_ALPHA2_START  (0x61)  // a
#define ASCII_ALPHA2_END    (0x7A)  // z
#define ASCII_DIGIT_START   (0x30)  // 0
#define ASCII_DIGIT_END     (0x39)  // 9
#define ASCII_HYPHEN        (0x2D)  // -
#define ASCII_PERIOD        (0x2E)  // .
#define ASCII_UNDERSCORE    (0x5F)  // _
#define ASCII_TILDA         (0x7E)  // ~

static int valueFromHexChar(char hex) {
    if ('0' <= hex && hex <= '9') {
        return hex - '0';
    } else if ('A' <= hex && hex <= 'F') {
        return hex - 'A' + 10;
    } else if ('a' <= hex && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        return 0;
    }
}

static char hexCharFromValue(unsigned int value) {
    if (16 <= value)
        return '0';
    return "0123456789ABCDEF"[value];
}

static void percentEncode(
    char* encoded,
    size_t encodedLength,
    const char* source) {
    char temp;
    encodedLength--;
    while (*source && encodedLength) {
        temp = *source;
        if ((ASCII_ALPHA1_START <= temp && temp <= ASCII_ALPHA1_END)
            || (ASCII_ALPHA2_START <= temp && temp <= ASCII_ALPHA2_END)
            || (ASCII_DIGIT_START <= temp && temp <= ASCII_DIGIT_END)
            || temp == ASCII_HYPHEN
            || temp == ASCII_PERIOD
            || temp == ASCII_UNDERSCORE
            || temp == ASCII_TILDA) {
            *(encoded++) = temp;
        } else {
            *(encoded++) = '%';
            if (!(--encodedLength)) break;
            *(encoded++) = hexCharFromValue((unsigned char)temp >> 4);
            if (!(--encodedLength)) break;
            *(encoded++) = hexCharFromValue(temp & 0x0F);
            encodedLength-=2;
        }
        source++;
        encodedLength--;
    }
    *encoded = '\0';
}

static void percentDecode(
    char* decoded,
    size_t decodedLength,
    const char* source) {
    decodedLength--;
    while (*source && decodedLength) {
        if (*source == '%') {
            if (*(source+1) == '\0') break;
            *(decoded++) =
                (valueFromHexChar(*(source + 1)) << 4) +
                valueFromHexChar(*(source + 2));
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
}

String::CPtr percentEncode(String::CPtr str, String::Encoding enc) {
    if (!str || str->length() == 0)
        return String::create();

    Buffer::Ptr buf = Buffer::create(str, enc);
    Size len = buf->length() * 3 + 1;
    char* encoded = new char[len];
    percentEncode(encoded, len, static_cast<const char*>(buf->data()));
    String::CPtr res = String::create(encoded, String::ASCII);
    delete [] encoded;
    return res;
}

String::CPtr percentDecode(String::CPtr str, String::Encoding enc) {
    if (!str || str->length() == 0)
        return String::create();

    Buffer::Ptr buf = Buffer::create(str, String::ASCII);
    Size len = buf->length() + 1;
    char* decoded = new char[len];
    percentDecode(decoded, len, static_cast<const char*>(buf->data()));
    String::CPtr res = String::create(decoded, enc);
    delete [] decoded;
    return res;
}

}  // namespace util
}  // namespace node
}  // namespace libj
