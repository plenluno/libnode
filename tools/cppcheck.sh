#!/bin/sh
cppcheck --xml --enable=all -f -q --includes-file=tools/cppcheck/include --suppressions-list=tools/cppcheck/suppress -DLIBJ_USE_BDW_GC include src
