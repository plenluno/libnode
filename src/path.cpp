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

static inline Boolean isSep(Char c) {
    return c == '\\' || c == '/';
}

// implement isAlpha because
// isalpha asserts the parameter is between -1 and 255,
// so isalpha(NO_POS) raises an exception.
static inline Boolean isAlpha(Char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline Size indexOfSep(String::CPtr str, Size from) {
    assert(str);
    Size len = str->length();
    for (Size i = from; i < len; i++) {
        if (isSep(str->charAt(i))) return i;
    }
    return NO_POS;
}

static inline Size lastIndexOfSep(String::CPtr str, Size from = NO_POS) {
    assert(str);
    Size len = str->length();
    if (!len) return NO_POS;

    if (from >= len) from = len - 1;

    Size i = from;
    while (1) {
        if (isSep(str->charAt(i))) return i;
        if (i) {
            i--;
        } else {
            break;
        }
    }
    return NO_POS;
}
#else
static const Char SEP = '/';

static inline Boolean isSep(Char c) {
    return c == SEP;
}

static inline Size indexOfSep(String::CPtr str, Size from) {
    return str->indexOf(SEP, from);
}

static inline Size lastIndexOfSep(String::CPtr str, Size from = NO_POS) {
    return str->lastIndexOf(SEP, from);
}
#endif

static inline String::CPtr trimSeps(String::CPtr str) {
    assert(str);
    Size len = str->length();
    if (!len) return str;

    Size i = len - 1;
    while (1) {
        if (!isSep(str->charAt(i))) {
            return str->substring(0, i + 1);
        }
        if (i) {
            i--;
        } else {
            break;
        }
    }
    return str->substring(0, 1);
}

String::CPtr normalize(String::CPtr path) {
    LIBJ_STATIC_SYMBOL_DEF(symCurrent, ".");
    LIBJ_STATIC_SYMBOL_DEF(symParent,  "..");
    LIBJ_STATIC_SYMBOL_DEF(symNull,    "null");

    if (!path) return symNull;

    Boolean absolute = false;
    Boolean endsWithSep = false;
    typedef TypedJsArray<String::CPtr> StringArray;
    StringArray::Ptr dirs = StringArray::create();
    Size len = path->length();
    for (Size i = 0; i < len;) {
        Size idx = indexOfSep(path, i);
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
    char* r = getcwd(dir, kMax);
    if (!r) return String::null();

    if (!paths) return String::create(dir);

    StringBuilder::Ptr resolved = StringBuilder::create();
    resolved->appendStr(dir);
    Size len = paths->length();
    for (Size i = 0; i < len; i++) {
        String::CPtr path = paths->getCPtr<String>(i);
        if (path) {
            if (isSep(path->charAt(0))) {
                resolved = StringBuilder::create();
#ifdef LIBJ_PF_WINDOWS
                // append the drive letter
                resolved->appendChar(dir[0]);
                resolved->appendChar(dir[1]);
            } else if (isAlpha(path->charAt(0)) && path->charAt(1) == ':') {
                resolved = StringBuilder::create();
#endif
            } else if (!path->isEmpty()) {
                resolved->appendChar(SEP);
            }
            resolved->appendStr(path);
        }
    }
    return trimSeps(normalize(resolved->toString()));
}

String::CPtr relative(String::CPtr from, String::CPtr to) {
    // TODO(plenluno): implement
    return String::create();
}

String::CPtr dirname(String::CPtr path) {
    LIBJ_STATIC_SYMBOL_DEF(symCurrent, ".");

    if (!path) return symCurrent;

    path = trimSeps(path);
    String::CPtr base = basename(path);
    Size baseLen = base->length();
    Size pathLen = path->length();
    if (baseLen == pathLen) {
        return symCurrent;
    } else {
        Size sepPos = pathLen - baseLen - 1;
        assert(isSep(path->charAt(sepPos)));
        if (sepPos) {
            return path->substring(0, sepPos);
        } else {
            return path->substring(0, 1);
        }
    }
}

String::CPtr basename(String::CPtr path) {
    LIBJ_STATIC_SYMBOL_DEF(symNull, "null");

    if (path) {
        path = trimSeps(path);
    } else {
        return symNull;
    }

    Size lastIndex = lastIndexOfSep(path);
    if (lastIndex == NO_POS) {
        return path;
    } else {
        return path->substring(lastIndex + 1);
    }
}

String::CPtr extname(String::CPtr path) {
    if (!path) return String::create();

    String::CPtr base = basename(path);
    if (base->charAt(0) == '.') return String::create();

    Size lastIndex = base->lastIndexOf('.');
    if (lastIndex == NO_POS) {
        return String::create();
    } else {
        return base->substring(lastIndex);
    }
}

String::CPtr sep() {
    static const char s[] = { static_cast<char>(SEP), 0 };
    LIBJ_STATIC_SYMBOL_DEF(symSep, s);
    return symSep;
}

String::CPtr delimiter() {
#ifdef LIBJ_PF_WINDOWS
    LIBJ_STATIC_SYMBOL_DEF(symDelimiter, ";");
#else
    LIBJ_STATIC_SYMBOL_DEF(symDelimiter, ":");
#endif
    return symDelimiter;
}

}  // namespace path
}  // namespace node
}  // namespace libj
