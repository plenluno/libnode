#!/bin/sh
tools/cpplint/cpplint.py --filter=-runtime/explicit,-runtime/threadsafe_fn,-readability/streams,-build/include_what_you_use --prefix=libnode --strip=4 `find exts -type f`
