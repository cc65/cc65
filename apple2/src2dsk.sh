#!/bin/sh
#
# Purpose: Assemble, Link, & Copy a binary to a DOS 3.3 .DSK image without all the cc65 library crap.
# Note: Leave off the extension for 'sourcefile.s'
# Usage: src2dsk.sh {sourcefile}
# Example: 
# 1. src2dsk.sh barebones
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
# Requires: a2tools
# * ftp://ftp.apple.asimov.net/pub/apple_II/utility/
# * http://slackbuilds.org/repository/14.1/system/a2tools/

cc65dir=../bin

# http://www.cc65.org/doc/ca65-2.html#ss2.2
if [[ -z ${cc65dir} ]]; then
    echo "Error: 'cc65dir' not set, should point to directory containing 'ca65', 'ld65'"
    return
else
    SRC=${1}.s
    OBJ=${1}.o
    BIN=${1}.bin

    ASM_FLAGS="--cpu 65c02"
    LINK_FLAGS="-C apple2bin.cfg"

    #DEBUG=echo
    ${DEBUG} ${cc65dir}/ca65 ${ASM_FLAGS}           -o ${OBJ} ${SRC}
    ${DEBUG} ${cc65dir}/ld65 ${LINK_FLAGS} -o ${BIN}   ${OBJ}

    # We need to uppercase the file name for a DOS 3.3 DSK
    # The ${1,,} is a Bash 4.0 uppercase extension so we can't use that
    # Likewise, GNU sed 's/.*/\L&/g' doesn't work on OSX (BSD)
    A2FILE=`echo "${1}" | awk '{print toupper($0)}'`
    a2rm      ${1}.DSK ${A2FILE}
    a2in -r b ${1}.DSK ${A2FILE} ${BIN} 
fi

