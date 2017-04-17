#!/bin/bash
#
# Purpose: Assemble, Link, & Copy a binary to a DOS 3.3 .DSK image without all the cc65 library crap.
# Usage: src2dsk.sh {sourcefile}
#
# Example:
# 1. src2dsk.sh barebones.s
#
#    foo.s   <- input assembly source file
#    foo.o   <- output of assembler
#    foo.bin <- output of linker
#    foo.dsk <- DOS 3.3 disk contaning binary 'FOO'
#
# 2. Mount 'barebones.dsk' in your emulator
#
# If you try BRUN'ing the file the RTS won't exit to DOS 3.3 / BASIC properly.
# A simple work-around is to BLOAD, then run it.
#   3. BLOAD BAREBONES
#   4. CALL -151
#   5. AA72.AA73
#   6  Use whatever addres is displayed (bytes are swapped):
#      1000G
#
# The 'barebones.s' exits via 'JMP $3D0' to warmstart DOS.
#
# You can get a blank DSK here
# * ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/

## wget ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/emptyDSK_Dos33.zip
# curl -# -o EmptyDSK_DOS33.zip ftp://ftp.apple.asimov.net/pub/apple_II/images/masters/emptyDSK_Dos33.zip
#
# I've included an 'empty.dsk' in the repo. for convenience
COPY=cp
DEL=rm

# Verify we have a .s file !
    if [[ -z ${1} ]]; then
        echo "ERROR: need an assembly source file (e.g. foo.s) to build from!"
        exit 1
    fi

# Verify we can find toolchain
# http://www.cc65.org/doc/ca65-2.html#ss2.2
    if [[ -z ${cc65dir} ]]; then
        echo "INFO: 'cc65dir' not set, should point to directory containing 'ca65', 'ld65'"
        cc65dir=../bin
        echo "INFO: Trying '${cc65dir}' for assembler and linker"
    fi

# Assembler
    if [ ! -f ${cc65dir}/ca65 ]; then
        echo "ERROR: Couldn't find assembler 'ca65' !"
        exit 1
    fi

# Linker
    if [ ! -f ${cc65dir}/ld65 ]; then
        echo "ERROR: Couldn't find linker 'ld65' !"
        exit 1
    fi

# Verify we can find a2tools
    if [ ! -f a2in ]; then
        echo "WARNING: Missing a2tools 'a2in', attempting to build"
        echo "INFO: Compiling 'a2tools' ..."
        echo "  gcc -DUNIX a2tools.c -o a2in"
        gcc -DUNIX a2tools.c -o a2in
        if [[ -f a2in ]]; then
            echo "... success!"
        else
            echo "ERROR: a2tools missing: 'a2in'"
            echo " "
            echo "The original tools can be found here:"
            echo " * ftp://ftp.apple.asimov.net/pub/apple_II/utility/"
            echo " * http://slackbuilds.org/repository/14.1/system/a2tools/"
            echo "To download:"
            echo "   curl -o a2tools.zip ftp://ftp.apple.asimov.net/pub/apple_II/utility/a2tools.zip"
            echo "This repo. contains a copy but was unable to compile it."
            exit 1
        fi
    fi

# Filenames and extensions
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

# Assemble & Link
    DEBUG=
    #DEBUG=echo
    ${DEBUG} ${DEL}                          ${BIN}   ${OBJ}
    ${DEBUG} ${cc65dir}/ca65 ${ASM_FLAGS}          -o ${OBJ} ${SRC}
    ${DEBUG} ${cc65dir}/ld65 ${LNK_FLAGS} -o ${BIN}   ${OBJ}

# Copy to .DKS
    # We need to uppercase the file name for a DOS 3.3 DSK
    # The ${1,,} is a Bash 4.0 uppercase extension so we can't use that
    # Likewise, GNU sed 's/.*/\L&/g' doesn't work on OSX (BSD)
    A2FILE=`echo "${FILE}" | awk '{print toupper($0)}'`
    ${COPY}  empty.dsk ${FILE}.dsk
    # If you want to keep an existing disk
    # you will want to first remove the old version on .DSK
    #${DEBUG} a2rm      ${FILE}.dsk ${A2FILE}
    ${DEBUG} a2in -r b ${FILE}.dsk ${A2FILE} ${BIN}

# Done!
    echo "INFO: Created: ${FILE}.dsk"

