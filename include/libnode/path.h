// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_PATH_H_
#define LIBNODE_PATH_H_

#include <libj/js_array.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace path {

String::CPtr normalize(String::CPtr path);

String::CPtr join(JsArray::CPtr paths);

String::CPtr resolve(JsArray::CPtr paths);

String::CPtr relative(String::CPtr from, String::CPtr to);

String::CPtr dirname(String::CPtr path);

String::CPtr basename(String::CPtr path);

String::CPtr extname(String::CPtr path);

String::CPtr sep();

String::CPtr delimiter();

}  // namespace path
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_PATH_H_
