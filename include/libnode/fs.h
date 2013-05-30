// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_FS_H_
#define LIBNODE_FS_H_

#include <libnode/buffer.h>
#include <libnode/fs/stats.h>

#include <libj/js_date.h>
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

enum Type {
    FILE,
    DIR,
    JUNCTION
};

void stat(
    String::CPtr path,
    JsFunction::Ptr callback);

void fstat(
    const Value& fd,
    JsFunction::Ptr callback);

void lstat(
    String::CPtr path,
    JsFunction::Ptr callback);

void chown(
    String::CPtr path,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback);

void fchown(
    const Value& fd,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback);

void chmod(
    String::CPtr path,
    Int mode,
    JsFunction::Ptr callback);

void fchmod(
    const Value& fd,
    Int mode,
    JsFunction::Ptr callback);

void truncate(
    String::CPtr path,
    Size len,
    JsFunction::Ptr callback);

void ftruncate(
    const Value& fd,
    Size len,
    JsFunction::Ptr callback);

void utimes(
    String::CPtr path,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback);

void utimes(
    String::CPtr path,
    JsDate::CPtr atime,
    JsDate::CPtr mtime,
    JsFunction::Ptr callback);

void futimes(
    const Value& fd,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback);

void futimes(
    const Value& fd,
    JsDate::CPtr atime,
    JsDate::CPtr mtime,
    JsFunction::Ptr callback);

void fsync(
    const Value& fd,
    JsFunction::Ptr callback);

void mkdir(
    String::CPtr path,
    JsFunction::Ptr callback);

void mkdir(
    String::CPtr path,
    Int mode,
    JsFunction::Ptr callback);

void rmdir(
    String::CPtr path,
    JsFunction::Ptr callback);

void readdir(
    String::CPtr path,
    JsFunction::Ptr callback);

void rename(
    String::CPtr oldPath,
    String::CPtr newPath,
    JsFunction::Ptr callback);

void link(
    String::CPtr srcPath,
    String::CPtr dstPath,
    JsFunction::Ptr callback);

void symlink(
    String::CPtr srcPath,
    String::CPtr dstPath,
    JsFunction::Ptr callback);

void symlink(
    String::CPtr srcPath,
    String::CPtr dstPath,
    node::fs::Type type,
    JsFunction::Ptr callback);

void unlink(
    String::CPtr path,
    JsFunction::Ptr callback);

void readlink(
    String::CPtr path,
    JsFunction::Ptr callback);

void realpath(
    String::CPtr path,
    JsFunction::Ptr callback);

void realpath(
    String::CPtr path,
    JsObject::Ptr cache,
    JsFunction::Ptr callback);

void open(
    String::CPtr path,
    Flag flag,
    JsFunction::Ptr callback);

void open(
    String::CPtr path,
    Flag flag,
    Int mode,
    JsFunction::Ptr callback);

void close(
    const Value& fd,
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

void appendFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback);

}  // namespace fs
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_FS_H_
