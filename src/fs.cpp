// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/detail/fs.h>

namespace libj {
namespace node {
namespace fs {

void stat(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::stat(path, callback);
}

void fstat(
    const Value& fd,
    JsFunction::Ptr callback) {
    node::detail::fs::fstat(fd, callback);
}

void lstat(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::lstat(path, callback);
}

void chown(
    String::CPtr path,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback) {
    node::detail::fs::chown(path, uid, gid, callback);
}

void fchown(
    const Value& fd,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback) {
    node::detail::fs::fchown(fd, uid, gid, callback);
}

void chmod(
    String::CPtr path,
    Int mode,
    JsFunction::Ptr callback) {
    node::detail::fs::chmod(path, mode, callback);
}

void fchmod(
    const Value& fd,
    Int mode,
    JsFunction::Ptr callback) {
    node::detail::fs::fchmod(fd, mode, callback);
}

void truncate(
    String::CPtr path,
    Size len,
    JsFunction::Ptr callback) {
    node::detail::fs::truncate(path, len, callback);
}

void ftruncate(
    const Value& fd,
    Size len,
    JsFunction::Ptr callback) {
    node::detail::fs::ftruncate(fd, len, callback);
}

void utimes(
    String::CPtr path,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback) {
    node::detail::fs::utimes(path, atime, mtime, callback);
}

void utimes(
    String::CPtr path,
    JsDate::CPtr atime,
    JsDate::CPtr mtime,
    JsFunction::Ptr callback) {
    Double at = atime ? atime->getTime() : QUIET_NAN;
    Double mt = mtime ? mtime->getTime() : QUIET_NAN;
    node::detail::fs::utimes(path, at, mt, callback);
}

void futimes(
    const Value& fd,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback) {
    node::detail::fs::futimes(fd, atime, mtime, callback);
}

void futimes(
    const Value& fd,
    JsDate::CPtr atime,
    JsDate::CPtr mtime,
    JsFunction::Ptr callback) {
    Double at = atime ? atime->getTime() : QUIET_NAN;
    Double mt = mtime ? mtime->getTime() : QUIET_NAN;
    node::detail::fs::futimes(fd, at, mt, callback);
}

void fsync(
    const Value& fd,
    JsFunction::Ptr callback) {
    node::detail::fs::fsync(fd, callback);
}

void mkdir(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::mkdir(path, 511, callback);
}

void mkdir(
    String::CPtr path,
    Int mode,
    JsFunction::Ptr callback) {
    node::detail::fs::mkdir(path, mode, callback);
}

void rmdir(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::rmdir(path, callback);
}

void readdir(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::readdir(path, callback);
}

void rename(
    String::CPtr oldPath,
    String::CPtr newPath,
    JsFunction::Ptr callback) {
    node::detail::fs::rename(oldPath, newPath, callback);
}

void link(
    String::CPtr srcPath,
    String::CPtr dstPath,
    JsFunction::Ptr callback) {
    node::detail::fs::link(srcPath, dstPath, callback);
}

void symlink(
    String::CPtr srcPath,
    String::CPtr dstPath,
    JsFunction::Ptr callback) {
    node::detail::fs::symlink(srcPath, dstPath, FILE, callback);
}

void symlink(
    String::CPtr srcPath,
    String::CPtr dstPath,
    node::fs::Type type,
    JsFunction::Ptr callback) {
    node::detail::fs::symlink(srcPath, dstPath, type, callback);
}

void unlink(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::unlink(path, callback);
}

void readlink(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::readlink(path, callback);
}

void open(
    String::CPtr path,
    Flag flag,
    JsFunction::Ptr callback) {
    node::detail::fs::open(path, flag, 438, callback);
}

void open(
    String::CPtr path,
    Flag flag,
    UInt mode,
    JsFunction::Ptr callback) {
    node::detail::fs::open(path, flag, mode, callback);
}

void close(
    const Value& fd,
    JsFunction::Ptr callback) {
    node::detail::fs::close(fd, callback);
}

void read(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    node::detail::fs::read(fd, buffer, offset, length, position, callback);
}

void write(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    node::detail::fs::write(fd, buffer, offset, length, position, callback);
}

void readFile(
    String::CPtr path,
    JsFunction::Ptr callback) {
    node::detail::fs::readFile(path, callback);
}

void writeFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback) {
    node::detail::fs::writeFile(path, data, callback);
}

void appendFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback) {
    node::detail::fs::appendFile(path, data, callback);
}

}  // namespace fs
}  // namespace node
}  // namespace libj
