// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_FILE_SYSTEM_H_
#define LIBNODE_FILE_SYSTEM_H_

#include <libj/js_function.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace fs {

enum Flag {
    R,
    RS,
    RP,
    RSP,
    W,
    WX,
    WP,
    WXP,
    A,
    AX,
    AP,
    AXP
};

void open(String::CPtr path, Flag flag, JsFunction::Ptr callback);
void close(const Value& fd, JsFunction::Ptr callback);
void read(const Value &fd, const Value& buffer, Size offset,
    Size length, Size position, JsFunction::Ptr callback);
void stat(String::CPtr path, JsFunction::Ptr callback);
void readFile(String::CPtr path, JsFunction::Ptr callback);

}  // namespace fs
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_FILE_SYSTEM_H_
