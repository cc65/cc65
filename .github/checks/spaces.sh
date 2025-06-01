#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_PATH=.

cd $SCRIPT_PATH/../../

FILES=`find $CHECK_PATH -type f \( -name \*.inc -o -name Makefile -o -name \*.cfg -o -name \*.\[chs\] -o -name \*.mac -o -name \*.asm -o -name \*.sgml \) -print | grep -v "test/" | grep -v "libwrk/" | grep -v "testwrk/" | xargs grep -l ' $'`

cd $OLDCWD

if [ x"$FILES"x != xx ]; then
    echo "error: found dangling spaces in the following files:" >&2
    for n in $FILES; do
        echo $n >&2
    done
    exit -1
fi
