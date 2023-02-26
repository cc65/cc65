This document contains all kinds of information that you should know if you want to contribute to the cc65 project. Before you start, please read all of it. If something is not clear to you, please ask - this document is an ongoing effort and may well be incomplete.

(''Note:'' The word "must" indicates a requirement.  The word "should" indicates a recomendation.)

*this is work in progress and is constantly updated - if in doubt, please ask*

# generally

* You must obey these rules when contributing new code or documentation to cc65. We are well aware that not all existing code may respect all rules outlined here - but this is no reason for you not to respect them.
* One commit/patch/PR per issue. Do not mix several things unless they are very closely related.
* Sometimes when you make a PR, it may break completely unrelated tests. However, any PR is expected to merge cleanly with no failures. That means in practise that you are expected to fix/update the failing tests if required - for example this might be needed if you make changes to the compiler that changes the format of error- or warning messages. In that case you might have to update some reference files in the testbench. Obviously still check if that is actually the right thing to do ;)

# Codestyle rules

## All Sources

### Line endings

All files must only contain Unix style 'LF' line endings. Please configure your editors accordingly.

### TABs and spaces

This is an ongoing controversial topic - everyone knows that. However, the following is how we do it :)

* TAB characters must be expanded to spaces.
* 4 spaces per indention level (rather than 8) are preferred, especially if there are many different levels.
* No extra spaces at the end of lines.
* All text files must end with new-line characters.  Don't leave the last line "dangling".

The (bash) scripts used to check the above rules can be found in ```.github/check```. You can also run all checks using ```make check```.

### Identifiers and Symbol names

The C Standard defines certain identifiers and symbol names, which we can not use
in our code. Since it is not always obvious which parts of the library code will
actually end up in a linked program, the following applies to ALL of the library.

Any non standard identifier/symbol/function that is exported from source files,
or appears in header files:

* must not be in the "_symbol" form in C, or "__symbol" form in assembly.
* must start with (at least) two (C Code) or three (assembly code) underscores, unless the symbol appears in a non standard header file.

This is likely more than the standard dictates us to do - but it is certainly
standard compliant - and easy to remember.

Also see the discussion in https://github.com/cc65/cc65/issues/1796

### misc

* 80 characters is the desired maximum width of files.  But, it isn't a "strong" rule; sometimes, you will want to type longer lines, in order to keep the parts of expressions or comments together on the same line.
* You should avoid typing non-ASCII characters.
* If you change "normal" source code into comments, then you must add a comment about why that code is a comment.
* When you want to create a comment from several lines of code, you should use preprocessor lines, instead of ```/* */``` or "```;```".  Example:
<pre>
#if 0
    one ();
    two ();
    three = two () + one ();
#endif
</pre>
* You should type upper case characters for hex values.
* When you type zero-page addresses in hexadecimal, you should type two hex characters (after the hex prefix).  When you type non-zero-page addresses in hex, you should type four hex characters.
* When you type lists of addresses, it is a good idea to sort them in ascending numerical order.  That makes it easier for readers to build mental pictures of where things are in an address space.  And, it is easier to see how big the variables and buffers are. Example:
<pre>
xCoord := $0703
yCoord := $0705        ; (this address implies that xCoord is 16 bits)
cmdbuf := $0706        ; (this address implies that yCoord is 8 bits)
cmdlen := $0786        ; (this address implies that cmdbuf is 128 bytes)
color  := $0787
</pre>

## C Sources

The following is still very incomplete - if in doubt please look at existing sourcefiles and adapt to the existing style

* Your files should obey the C89 standard.
* We generally have a "no warnings" policy
* Warnings must not be hidden by using typecasts - fix the code instead
   * In printf-style functions use the PRIX64 (and similar) macros to deal with 64bit values
* The normal indentation width should be four spaces.
* You must use ANSI C comments (```/* */```); you must not use C++ comments (```//```).
* When you add functions to an existing file, you should separate them by the same number of blank lines that separate the functions that already are in that file.
* All function declarations must be followed by a comment block that tells at least briefly what the function does, what the parameters are, and what is returned. This comment must sit between the declaration and the function body, like this:
<pre>
int foo(int bar)
/* Add 1 to bar, takes bar and returns the result */
{
    return bar + 1;
}
</pre>
* When a function's argument list wraps around to a next line, you should indent that next line by either the normal width or enough spaces to align it with the arguments on the previous line.
* All declarations in a block must be at the beginning of that block.
* You should put a blank line between a list of local variable declarations and the first line of code.
* Always use curly braces even for single statements after ```if```, and the single statement should go into a new line.
* Use "cuddling" braces, ie the opening brace goes in the same line as the ```if```:
<pre>
if (foo > 42) {
    bar = 23;
}
</pre>
* Should the ```if``` statement be followed by an empty conditional block, there should be a comment telling why this is the case
<pre>
if (check()) {
    /* nothing happened, do nothing */
}
</pre>
* You must separate function names and parameter/argument lists by one space.
* When declaring/defining pointers, you must put the asterisk (```*```) next to the data type, with a space between it and the variable's name.  Examples:
<pre>
    int* namedPtr[5];
    char* nextLine (FILE* f);
</pre>

### Header files

Headers that belong to the standard library (libc) must conform with the C standard. That means:
* all non standard functions, or functions that only exist in a certain standard, should be in #ifdefs
    * the same is true for macros or typedefs
<pre>
#if __CC65_STD__ == __CC65_STD_C99__
/* stuff that only exists in C99 here */
#endif
#if __CC65_STD__ == __CC65_STD_CC65__
/* non standard stuff here */
#endif
</pre>
You can refer to Annex B of the ISO C99 standard ([here](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf) is the draft).

## Assembly Sources

* Op-code mnemonics must have lower-case letters.  The names of instruction macroes may have upper-case letters.
* Op-codes must use their official and commonly used mnemonics, ie bcc and bcs and not bgt and blt
* Hexadecimal number constants should be used except where decimal or binary numbers make much more sense in that constant's context.
* Hexadecimal letters should be upper-case.
* When you set two registers or two memory locations to an immediate 16-bit zero, you should use the expressions ```#<$0000``` and ```#>$0000``` (they make it obvious where you are putting the lower and upper bytes).
* If a function is declared to return a char-sized value, it actually must return an integer-sized value.  (When cc65 promotes a returned value, it sometimes assumes that the value already is an integer.) This must be done in one of the following ways:
<pre>
    lda #RETURN_VALUE
    ldx #0 ; return value is char
</pre>
or, if the value is 0, you can use:
<pre>
    lda #RETURN_VALUE
    .assert RETURN_VALUE = 0
    tax
</pre>
sometimes jumping to return0 could save a byte:
<pre>
    .assert RETURN_VALUE = 0
    jmp return 0
</pre>
* Functions, that are intended for a platform's system library, should be optimized as much as possible.
* Sometimes, there must be a trade-off between size and speed.  If you think that a library function won't be used often, then you should make it small.  Otherwise, you should make it fast.
* Comments that are put on the right side of instructions must be aligned (start in the same character columns).
* Assembly source fields (label, operation, operand, comment) should start ''after'' character columns that are multiples of eight (such as 1, 9, 17, 33, and 41).
  
## LinuxDoc Sources

* TAB characters must be expanded to spaces.
* All text files must end with new-line characters.  Don't leave the last line "dangling".
* 80 characters is the desired maximum width of files.
* You should avoid typing non-ASCII characters.

* You should put blank lines between LinuxDoc sections:
  * Three blank lines between ```<sect>``` sections.
  * Two blank lines between ```<sect1>``` sections.
  * One blank line between other sections.

# Library implementation rules

* By default the toolchain must output a "standard" binary for the platform, no emulator formats, no extra headers used by tools. If the resulting binaries can not be run as is on emulators or eg flash cartridges, the process of converting them to something that can be used with these should be documented in the user manual.
* Generally every function should live in a seperate source file - unless the functions are so closely related that splitting makes no sense.
* Source files should not contain commented out code - if they do, there should be a comment that explains why that commented out code exists.

# Makefile rules

* Makefiles must generally work on both *nix (ba)sh and windows cmd.exe.
* Makefiles must not use external tools that are not provided by the cc65 toolchain itself.

The only exception to the above are actions that are exclusive to the github actions - those may rely on bash and/or linux tools.

# Documentation rules

## User manual (LinuxDoc)

* This is the primary documentation.

## Wiki

* The Wiki is strictly for additional information that does not fit into the regular user manual (LinuxDoc). The wiki must not duplicate any information that is present in the user manual

# Roadmap / TODOs / open Ends

## Documentation

* the printf family of function does not completely implement all printf modifiers and does not behave as expected in some cases - all this should be documented in detail

## Compiler

* We need a way that makes it possible to feed arbitrary assembler code into the optimzer, so we can have proper tests for it

### Floating point support

The first step is implementing the datatype "float" as IEEE 754 floats. Help welcomed!

* WIP compiler/library changes are here: https://github.com/cc65/cc65/pull/1777

## Library

### name clashes in the library

see "Identifiers and Symbol names" above - not all identifiers have been checked
and renamed yet. The following is a list of those that still might need to be
fixed:

```
common

__argc                  libsrc/runtime/callmain.s libsrc/cbm610/mainargs.s libsrc/cx16/mainargs.s libsrc/plus4/mainargs.s libsrc/lynx/mainargs.s libsrc/c16/mainargs.s libsrc/geos-common/system/mainargs.s libsrc/sim6502/mainargs.s libsrc/c128/mainargs.s libsrc/vic20/mainargs.s libsrc/nes/mainargs.s libsrc/atari/getargs.s libsrc/apple2/mainargs.s libsrc/cbm510/mainargs.s libsrc/telestrat/mainargs.s libsrc/c64/mainargs.s libsrc/pet/mainargs.s libsrc/atmos/mainargs.s 
__argv                  libsrc/runtime/callmain.s libsrc/cbm610/mainargs.s libsrc/cx16/mainargs.s libsrc/plus4/mainargs.s libsrc/lynx/mainargs.s libsrc/c16/mainargs.s libsrc/geos-common/system/mainargs.s libsrc/sim6502/mainargs.s libsrc/c128/mainargs.s libsrc/vic20/mainargs.s libsrc/nes/mainargs.s libsrc/atari/getargs.s libsrc/apple2/mainargs.s libsrc/cbm510/mainargs.s libsrc/telestrat/mainargs.s libsrc/c64/mainargs.s libsrc/pet/mainargs.s libsrc/atmos/mainargs.s 
__cos                   libsrc/common/sincos.s
__ctypeidx              libsrc/common/ctype.s libsrc/common/ctypemask.s libsrc/geos-common/system/ctype.s libsrc/atari/ctype.s libsrc/cbm/ctype.s libsrc/atmos/ctype.s asminc/ctype_common.inc
__cwd                   libsrc/common/getcwd.s libsrc/common/_cwd.s libsrc/atari/initcwd.s libsrc/apple2/initcwd.s libsrc/apple2/initcwd.s libsrc/telestrat/initcwd.s libsrc/cbm/initcwd.s
__cwd_buf_size          libsrc/common/_cwd.s
__envcount              libsrc/common/searchenv.s libsrc/common/_environ.s libsrc/common/putenv.s libsrc/common/getenv.s
__environ               libsrc/common/searchenv.s libsrc/common/_environ.s libsrc/common/putenv.s libsrc/common/getenv.s
__envsize               libsrc/common/_environ.s libsrc/common/putenv.s
__fdesc                 libsrc/common/_fdesc.s libsrc/common/fopen.s
__filetab               libsrc/common/_fdesc.s libsrc/common/_file.s asminc/_file.inc
__fopen                 libsrc/common/fopen.s libsrc/common/_fopen.s
__printf                libsrc/common/vsnprintf.s libsrc/common/_printf.s libsrc/common/vfprintf.s libsrc/conio/vcprintf.s libsrc/pce/_printf.s
__scanf                 libsrc/common/_scanf.inc libsrc/common/vsscanf.s libsrc/conio/vcscanf.s
__sin                   libsrc/common/sincos.s
__sys                   libsrc/common/_sys.s libsrc/apple2/_sys.s
__sys_oserrlist         libsrc/common/stroserr.s libsrc/geos-common/system/oserrlist.s libsrc/atari/oserrlist.s libsrc/apple2/oserrlist.s libsrc/cbm/oserrlist.s libsrc/atmos/oserrlist.s
__syschdir              libsrc/common/chdir.s libsrc/atari/syschdir.s libsrc/apple2/syschdir.s libsrc/telestrat/syschdir.s libsrc/cbm/syschdir.s
__sysmkdir              libsrc/common/mkdir.s libsrc/atari/sysmkdir.s libsrc/apple2/sysmkdir.s libsrc/telestrat/sysmkdir.s
__sysremove             libsrc/common/remove.s libsrc/geos-common/file/sysremove.s libsrc/atari/sysremove.s libsrc/atari/sysrmdir.s libsrc/apple2/sysremove.s libsrc/apple2/sysrmdir.s libsrc/telestrat/sysremove.s libsrc/cbm/sysremove.s
__sysrename             libsrc/common/rename.s libsrc/geos-common/file/sysrename.s libsrc/atari/sysrename.s libsrc/apple2/sysrename.s libsrc/cbm/sysrename.s
__sysrmdir              libsrc/common/rmdir.s libsrc/atari/sysrmdir.s libsrc/apple2/sysrmdir.s
__sysuname              libsrc/common/uname.s libsrc/cbm610/sysuname.s libsrc/cx16/sysuname.s libsrc/plus4/sysuname.s libsrc/lynx/sysuname.s libsrc/c16/sysuname.s libsrc/geos-common/system/sysuname.s libsrc/c128/sysuname.s libsrc/creativision/sysuname.s libsrc/vic20/sysuname.s libsrc/nes/sysuname.s libsrc/atari/sysuname.s libsrc/apple2/sysuname.s libsrc/cbm510/sysuname.s libsrc/telestrat/sysuname.s libsrc/c64/sysuname.s libsrc/pet/sysuname.s libsrc/atari5200/sysuname.s libsrc/atmos/sysuname.s

apple2

__auxtype               libsrc/apple2/open.s
__datetime              libsrc/apple2/open.s
__dos_type              libsrc/apple2/dioopen.s libsrc/apple2/curdevice.s libsrc/apple2/mainargs.s libsrc/apple2/settime.s libsrc/apple2/getdevice.s libsrc/apple2/dosdetect.s libsrc/apple2/irq.s libsrc/apple2/open.s libsrc/apple2/mli.s libsrc/apple2/getres.s
__filetype              libsrc/apple2/open.s  libsrc/apple2/exehdr.s


atari

__defdev                libsrc/atari/posixdirent.s libsrc/atari/ucase_fn.s libsrc/atari/getdefdev.s
__dos_type              libsrc/atari/getargs.s libsrc/atari/exec.s libsrc/atari/settime.s libsrc/atari/syschdir.s libsrc/atari/dosdetect.s libsrc/atari/is_cmdline_dos.s libsrc/atari/sysrmdir.s libsrc/atari/gettime.s libsrc/atari/lseek.s libsrc/atari/getres.s libsrc/atari/getdefdev.s
__do_oserror            libsrc/atari/posixdirent.s libsrc/atari/do_oserr.s libsrc/atari/serref.s libsrc/atari/read.s libsrc/atari/write.s libsrc/atari/close.s
__getcolor              libsrc/atari/setcolor.s
__getdefdev             libsrc/atari/getdefdev.s
__graphics              libsrc/atari/graphics.s
__inviocb               libsrc/atari/serref.s libsrc/atari/ser/atrrdev.s libsrc/atari/inviocb.s libsrc/atari/read.s libsrc/atari/write.s libsrc/atari/lseek.s libsrc/atari/close.s
__is_cmdline_dos        libsrc/atari/is_cmdline_dos.s libsrc/atari/doesclrscr.s
__rest_vecs             libsrc/atari/savevec.s
__rwsetup               libsrc/atari/rwcommon.s libsrc/atari/read.s libsrc/atari/write.s
__save_vecs             libsrc/atari/savevec.s
__scroll                libsrc/atari/scroll.s
__setcolor              libsrc/atari/setcolor.s
__setcolor_low          libsrc/atari/setcolor.s
__sio_call              libsrc/atari/diowritev.s libsrc/atari/diopncls.s libsrc/atari/siocall.s libsrc/atari/diowrite.s libsrc/atari/dioread.s


cbm

__cbm_filetype          libsrc/cbm/cbm_filetype.s asminc/cbm_filetype.in
__dirread               libsrc/cbm/dir.inc libsrc/cbm/dir.s
__dirread1              libsrc/cbm/dir.inc libsrc/cbm/dir.s


lynx

__iodat                 libsrc/lynx/lynx-cart.s libsrc/lynx/bootldr.s libsrc/lynx/extzp.s libsrc/lynx/crt0.s libsrc/lynx/extzp.inc
__iodir                 libsrc/lynx/extzp.s libsrc/lynx/crt0.s libsrc/lynx/extzp.inc
__sprsys                libsrc/lynx/tgi/lynx-160-102-16.s libsrc/lynx/extzp.s libsrc/lynx/crt0.s libsrc/lynx/extzp.inc
__viddma                libsrc/lynx/tgi/lynx-160-102-16.s libsrc/lynx/extzp.s libsrc/lynx/crt0.s libsrc/lynx/extzp.inc


pce

__nmi                   libsrc/pce/irq.s libsrc/pce/crt0.s
```

## Test suite

* specific tests to check the optimizer (rather than the codegenerator) are needed.
* we need more specific tests to check standard conformance of the library headers
