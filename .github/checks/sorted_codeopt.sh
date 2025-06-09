#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_FILE=../../src/cc65/codeopt.c

SORT_OPT=-u

grep "^static OptFunc " $CHECK_FILE | \
    sed -e 's:.*"\(.*\)",.*:\1:g' > .a.tmp

if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
    echo "error: OptFunc table is empty"
    exit -1
fi

LC_COLLATE=C sort $SORT_OPT .a.tmp > .b.tmp

if cmp --silent -- .a.tmp .b.tmp; then
    echo "static OptFunc definitions OK"
else
    echo "error: static OptFunc definitions are not sorted."
    diff -y .a.tmp .b.tmp
    exit -1
fi

awk '/static OptFunc\* OptFuncs\[\] = {/{flag=1;next}/}/{flag=0}flag' $CHECK_FILE | \
    sed -e 's:.*&D\(.*\),:\1:g' > .a.tmp

if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
    echo "error: OptFuncs table is empty"
    exit -1
fi

if cmp --silent -- .a.tmp .b.tmp; then
    echo "static OptFuncs* OptFuncs[] definitions OK"
else
    echo "error: static OptFuncs* OptFuncs[] definitions are not sorted."
    diff -y .a.tmp .b.tmp
    exit -1
fi

rm -rf .a.tmp .b.tmp
