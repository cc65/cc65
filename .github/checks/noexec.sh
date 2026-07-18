#! /bin/bash

OLDCWD=`pwd`
SCRIPT_PATH=`dirname "$0"`
CHECK_PATH=.
FOUND=0

cd "$SCRIPT_PATH/../../"

while IFS= read -r FILE; do
    if [ -x "$FILE" ]; then
        if [ $FOUND -eq 0 ]; then
            echo "error: executable flag is set for the following files:" >&2
        fi
        echo "$FILE" >&2
        FOUND=1
    fi
done < <(find $CHECK_PATH -type f \( -name \*.inc -o -name Makefile -o -name \*.cfg -o -name \*.\[chs\] -o -name \*.mac -o -name \*.asm -o -name \*.sgml \) -print)

cd "$OLDCWD"

exit $FOUND
