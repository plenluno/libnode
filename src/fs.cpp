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

}  // namespace fs
}  // namespace node
}  // namespace libj
