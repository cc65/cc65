#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_PATH=.

cd $SCRIPT_PATH/../../

nl='
'
nl=$'\n'
r1="${nl}$"
FILES=`find $CHECK_PATH -type f \( -name \*.inc -o -name Makefile -o -name \*.cfg -o -name \*.\[chs\] -o -name \*.mac -o -name \*.asm -o -name \*.sgml \) -print | while read f; do
    t=$(tail -c2 $f; printf x)
    [[ ${t%x} =~ $r1 ]] || echo "$f"
done`

cd $OLDCWD

if [ x"$FILES"x != xx ]; then
    echo "error: found following files that have no newline at the end:"
    for n in $FILES; do
        echo $n
    done
    exit -1
fi
