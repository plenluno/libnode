// Copyright (c) 2014 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/invoke.h>

namespace libj {
namespace node {

class GTestAdd : LIBJ_JS_FUNCTION(GTestAdd)
 public:
    Value operator()(JsArray::Ptr args) {
        if (!args) {
            return Error::create(Error::ILLEGAL_ARGUMENT);
        }

        Int sum = 0;
        for (Size i = 0; i < args->length(); i++) {
            int x;
            if (to<Int>(args->get(i), &x)) {
                sum += x;
            } else {
                return Error::create(Error::ILLEGAL_ARGUMENT);
            }
        }
        return sum;
    }

    static GTestAdd::Ptr create() {
        return Ptr(new GTestAdd());
    }
};

TEST(GTestInvoke, TestInvoke) {
    GTestAdd::Ptr add = GTestAdd::create();

    ASSERT_TRUE(invoke(add).instanceof(Type<Error>::id()));
    ASSERT_TRUE(invoke(add, 1).equals(1));
    ASSERT_TRUE(invoke(add, 1, 2).equals(3));
    ASSERT_TRUE(invoke(add, 1, 2, 3).equals(6));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4).equals(10));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4, 5).equals(15));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4, 5, 6).equals(21));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4, 5, 6, 7).equals(28));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4, 5, 6, 7, 8).equals(36));
    ASSERT_TRUE(invoke(add, 1, 2, 3, 4, 5, 6, 7, 8, 9).equals(45));
}

}  // namespace node
}  // namespace libj
