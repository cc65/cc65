[Windows Snapshot](http://sourceforge.net/projects/cc65/files/cc65-snapshot-win32.zip)

[Documentation](http://cc65.github.io/doc)

[Wiki](http://github.com/cc65/wiki/wiki)

[![Build Status](https://api.travis-ci.org/cc65/cc65.svg?branch=master)](https://travis-ci.org/cc65/cc65/builds)

cc65 is a complete cross development package for 65(C)02 systems, including
a powerful macro assembler, a C compiler, linker, librarian and several
other tools.

cc65 has C and runtime library support for many of the old 6502 machines,
including

- the following Commodore machines:
  - VIC20
  - C16/C116 and Plus/4
  - C64
  - C128
  - CBM 510 (aka P500)
  - the 600/700 family
  - newer PET machines (not 2001).
- the Apple ][+ and successors.
- the Atari 8 bit machines.
- the Atari 5200 console.
- GEOS for the C64, C128 and Apple //e.
- the NEC PC-Engine (aka TurboGrafx-16).
- the Nintendo Entertainment System (NES) console.
- the Supervision console.
- the Oric Atmos.
- the Lynx console.
- the Ohio Scientific Challenger 1P

The libraries are fairly portable, so creating a version for other 6502s
shouldn't be too much work.

# Apple 2 standalone barebones assembly without C libraries.

1. You need to generate a 4 bytes DOS 3.3 binary file prefix

 Place these 4 lines at the beginning of your source file:

            __MAIN = $1000       ; Apple DOS 3.3 binary file 4 byte prefix header
            .word __MAIN         ; 2 byte BLAOD address
            .word __END - __MAIN ; 2 byte BLOAD size
            .org  __MAIN         ; .org must come after header else offsets are wrong

 Or the simpler 2-liner:

            __MAIN = $1000
            .include "dos33.h"   ; Apple DOS 3.3 binary file 4 byte prefix header


2. If you want the ability to BRUN your binary and return to DOS when done you need to use `JMP $3D0` instead of an `RTS`. (If you really want to use `RTS` you will first need to BLOAD your binary and then run it manually with an Applesoft `CALL` or monitor `####G` command.)

3. If you use `a2tools` you need to use a2tools in "raw" mode

4. If you need text with the high-bit set use the macro ASC in [barebones.s](apple2/barebones.s)

* See the directory [apple2](apple2/) for more details.

