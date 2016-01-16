#!/bin/sh
#
# Purpose: Assemble, Link, & Copy a binary to a DOS 3.3 .DSK image without all the cc65 library crap.
# Usage: src2dsk.sh {sourcefile}
#
# Example: 
# 1. src2dsk.sh barebones.s
#
#    foo.s   <- original assembly source file
#    foo.o   <- output of assembler
#    foo.bin <- output of linker
#    foo.bin is "copied" to 'foo.dsk' as "FOO"
# 2. Mount 'barebones.dsk' in your emulator
#
# If you try BRUN'ing the file the RTS won't exit to BASIC properly.
# A simple work-around is to BLOAD, then run it.
# 3. BLOAD BAREBONES
# 4. CALL -151
# 5. AA72.AA73
# 6  Use whatever addres is displayed (bytes are swapped):
#    1000G
#
# You can get a blank DSK here
# * ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/
# wget ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/emptyDSK_Dos33.zip
# curl -# -o EmptyDSK_DOS33.zip ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/emptyDSK_Dos33.zip
#
cc65dir=../bin
COPY=cp
DEL=rm

# http://www.cc65.org/doc/ca65-2.html#ss2.2
if [[ -z ${cc65dir} ]]; then
    echo "Error: 'cc65dir' not set, should point to directory containing 'ca65', 'ld65'"
    return
else
    #http://stackoverflow.com/questions/965053/extract-filename-and-extension-in-bash
    # Get filename without path
    # Get filename without extension
    FILENAME=$(basename "${1}")
    FILE="${FILENAME%%.*}"

    SRC=${FILE}.s
    OBJ=${FILE}.o
    BIN=${FILE}.bin

    ASM_FLAGS="--cpu 65c02"
    LNK_FLAGS="-C apple2bin.cfg"

    DEBUG=
    #DEBUG=echo
    ${DEBUG} ${DEL}                          ${BIN}   ${OBJ}
    ${DEBUG} ${cc65dir}/ca65 ${ASM_FLAGS}          -o ${OBJ} ${SRC}
    ${DEBUG} ${cc65dir}/ld65 ${LNK_FLAGS} -o ${BIN}   ${OBJ}

    # We need to uppercase the file name for a DOS 3.3 DSK
    # The ${1,,} is a Bash 4.0 uppercase extension so we can't use that
    # Likewise, GNU sed 's/.*/\L&/g' doesn't work on OSX (BSD)
    if [[ -f a2rm && -f a2in ]]; then
        A2FILE=`echo "${FILE}" | awk '{print toupper($0)}'`
        ${COPY}  empty.dsk ${FILE}.DSK
        #${DEBUG} a2rm      ${FILE}.DSK ${A2FILE}
        ${DEBUG} a2in -r b ${FILE}.DSK ${A2FILE} ${BIN}
    else
        echo "ERROR: a2tools missing: 'a2rm' and 'a2in'"
        echo " "
        echo "It can be found here:"
        echo " * ftp://ftp.apple.asimov.net/pub/apple_II/utility/"
        echo " * http://slackbuilds.org/repository/14.1/system/a2tools/"
        echo "To download:"
        echo "   curl -o a2tools.zip ftp://ftp.apple.asimov.net/pub/apple_II/utility/a2tools.zip"
    fi
fi

