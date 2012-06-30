// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/fs/stats.h"

namespace libj {
namespace node {
namespace fs {

const String::CPtr STAT_DEV = String::create("dev");
const String::CPtr STAT_INO = String::create("ino");
const String::CPtr STAT_MODE = String::create("mode");
const String::CPtr STAT_NLINK = String::create("nlink");
const String::CPtr STAT_UID = String::create("uid");
const String::CPtr STAT_GID = String::create("gid");
const String::CPtr STAT_RDEV = String::create("rdev");
const String::CPtr STAT_SIZE = String::create("size");
const String::CPtr STAT_BLKSIZE = String::create("blksize");
const String::CPtr STAT_BLOCKS = String::create("blocks");
const String::CPtr STAT_ATIME = String::create("atime");
const String::CPtr STAT_MTIME = String::create("mtime");
const String::CPtr STAT_CTIME = String::create("ctime");

}  // namespace events
}  // namespace node
}  // namespace libj
