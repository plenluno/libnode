// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/path.h>

#include <libj/symbol.h>
#include <libj/string_builder.h>
#include <libj/typed_js_array.h>

#ifdef LIBJ_PF_WINDOWS
# include <libj/platform/windows.h>
#else
# include <unistd.h>
#endif

#include <assert.h>

namespace libj {
namespace node {
namespace path {

#ifdef LIBJ_PF_WINDOWS
static const Char SEP = '\\';
#else
static const Char SEP = '/';
#endif

String::CPtr normalize(String::CPtr path) {
    LIBJ_STATIC_SYMBOL_DEF(symCurrent, ".");
    LIBJ_STATIC_SYMBOL_DEF(symParent,  "..");

    if (!path) return String::create();

    Boolean absolute = false;
    Boolean endsWithSep = false;
    typedef TypedJsArray<String::CPtr> StringArray;
    StringArray::Ptr dirs = StringArray::create();
    Size len = path->length();
    for (Size i = 0; i < len;) {
        Size idx = path->indexOf(SEP, i);
        if (idx == 0) {
            absolute = true;
        } else if (idx != i) {
            String::CPtr dir;
            if (idx == NO_POS) {
                dir = path->substring(i);
            } else {
                dir = path->substring(i, idx);
            }
            if (dir->equals(symParent)) {
                Size numDirs = dirs->size();
                if (numDirs > 0 &&
                    !dirs->getTyped(numDirs - 1)->equals(symParent)) {
                    dirs->removeTyped(numDirs - 1);
                } else {
                    dirs->addTyped(dir);
                }
            } else if (!dir->equals(symCurrent)) {
                dirs->addTyped(dir);
            }
        }

        if (idx == NO_POS) {
            endsWithSep = false;
            i = len;
        } else {
            endsWithSep = true;
            i = idx + 1;
        }
    }

    StringBuilder::Ptr normal = StringBuilder::create();
    if (absolute)
        normal->appendChar(SEP);
    Size numDirs = dirs->size();
    for (Size i = 0; i < numDirs; i++) {
        if (i) normal->appendChar(SEP);
        normal->appendStr(dirs->getTyped(i));
    }
    if (numDirs > 0 && endsWithSep)
        normal->appendChar(SEP);
    if (normal->length() == 0) {
        return symCurrent;
    } else {
        return normal->toString();
    }
}

String::CPtr join(JsArray::CPtr paths) {
    LIBJ_STATIC_SYMBOL_DEF(symCurrent, ".");

    if (!paths) return symCurrent;

    StringBuilder::Ptr joined = StringBuilder::create();
    Size len = paths->length();
    Boolean first = true;
    for (Size i = 0; i < len; i++) {
        String::CPtr path = paths->getCPtr<String>(i);
        if (path) {
            if (first) {
                first = false;
            } else {
                joined->appendChar(SEP);
            }
            joined->appendStr(path);
        }
    }
    return normalize(joined->toString());
}

String::CPtr resolve(JsArray::CPtr paths) {
    const Size kMax = 8192;
    char dir[kMax];
    getcwd(dir, kMax);

    if (!paths) return String::create(dir);

    StringBuilder::Ptr resolved = StringBuilder::create();
    resolved->appendStr(dir);
    Size len = paths->length();
    for (Size i = 0; i < len; i++) {
        String::CPtr path = paths->getCPtr<String>(i);
        if (path) {
            if (path->charAt(0) == SEP) {
                resolved = StringBuilder::create();
            } else if (!path->isEmpty()) {
                resolved->appendChar(SEP);
            }
            resolved->appendStr(path);
        }
    }
    return normalize(resolved->toString());
}

String::CPtr relative(String::CPtr from, String::CPtr to) {
    // TODO(plenluno): implement
    return String::create();
}

String::CPtr dirname(String::CPtr path) {
    LIBJ_STATIC_SYMBOL_DEF(symCurrent, ".");

    if (!path) return symCurrent;

    String::CPtr base = basename(path);
    Size baseLen = base->length();
    Size pathLen = path->length();
    if (baseLen == pathLen) {
        return symCurrent;
    } else {
        Size sepPos = pathLen - baseLen - 1;
        assert(path->charAt(sepPos) == SEP);
        if (sepPos) {
            return path->substring(0, sepPos);
        } else {  // path[0] is '/'
            return sep();
        }
    }
}

String::CPtr basename(String::CPtr path) {
    static const String::CPtr doubleSep = String::create(SEP, 2);

    if (!path) return String::create();

    Size len = path->length();
    if (!len) return String::create();

    if (path->equals(sep())) return String::create();
    if (path->equals(doubleSep)) return String::create();

    Size lastIndex;
    assert(len >= 1);
    if (path->charAt(len - 1) == SEP) {
        assert(len >= 2);
        Size from = len - 2;
        lastIndex = path->lastIndexOf(SEP, from);
    } else {
        lastIndex = path->lastIndexOf(SEP);
    }
    String::CPtr base;
    if (lastIndex == NO_POS) {
        base = path;
    } else {
        base = path->substring(lastIndex + 1);
    }
    return base;
}

String::CPtr extname(String::CPtr path) {
    if (!path) return String::create();

    String::CPtr base = basename(path);
    if (base->endsWith(sep())) return String::create();

    Size lastIndex = NO_POS;
    if (base->length() > 1)
        lastIndex = base->lastIndexOf('.');
    String::CPtr ext;
    if (lastIndex == NO_POS) {
        ext = String::create();
    } else {
        ext = base->substring(lastIndex);
    }
    return ext;
}

String::CPtr sep() {
    static const String::CPtr sep = String::create(SEP);
    return sep;
}

}  // namespace path
}  // namespace node
}  // namespace libj
