// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_FS_H_
#define LIBNODE_FS_H_

#include <libnode/buffer.h>
#include <libnode/fs/stats.h>

#include <libj/js_function.h>

namespace libj {
namespace node {
namespace fs {

enum Flag {
    R,
    RP,
    RS,
    RSP,
    W,
    WP,
    WX,
    WXP,
    A,
    AP,
    AX,
    AXP
};

void open(
    String::CPtr path,
    Flag flag,
    JsFunction::Ptr callback);

void open(
    String::CPtr path,
    Flag flag,
    UInt mode,
    JsFunction::Ptr callback);

void close(
    const Value& fd,
    JsFunction::Ptr callback);

void stat(
    String::CPtr path,
    JsFunction::Ptr callback);

void read(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback);

void write(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback);

void readFile(
    String::CPtr path,
    JsFunction::Ptr callback);

void writeFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback);

}  // namespace fs
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_FS_H_
