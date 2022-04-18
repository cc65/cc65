#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_PATH=.

cd $SCRIPT_PATH/../../

FILES=`find $CHECK_PATH -type f \( \( -name \*.inc -a \! -name Makefile.inc \) -o -name \*.cfg -o -name \*.\[chs\] -o -name \*.mac -o -name \*.asm -o -name \*.sgml \) -print | grep -v "libwrk/" | grep -v "testwrk/" | xargs grep -l $'\t'`

cd $OLDCWD

if [ x"$FILES"x != xx ]; then
    echo "error: found TABs in the following files:"
    for n in $FILES; do
        echo $n
    done
    exit -1
fi
