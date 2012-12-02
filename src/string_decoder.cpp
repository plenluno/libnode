// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/string_decoder.h>
#include <libnode/detail/string_decoder.h>

namespace libj {
namespace node {

StringDecoder::Ptr StringDecoder::create(Buffer::Encoding enc) {
    return Ptr(new detail::StringDecoder<StringDecoder>(enc));
}

}  // namespace node
}  // namespace libj
