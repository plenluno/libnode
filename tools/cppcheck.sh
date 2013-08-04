#!/bin/sh
cppcheck --xml --enable=all -f -q --includes-file=tools/cppcheck/include --suppressions-list=tools/cppcheck/suppress -DLIBJ_USE_BDWGC -DLIBNODE_USE_BDWGC include src example exts
