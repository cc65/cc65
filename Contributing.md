This document contains all kinds of information that you should know if you want to contribute to the cc65 project. Before you start, please read all of it. If something is not clear to you, please ask - this document is an ongoing effort and may well be incomplete.

(''Note:'' The word "must" indicates a requirement.  The word "should" indicates a recomendation.)

*this is work in progress and is constantly updated - if in doubt, please ask*

# generally

* You must obey these rules when contributing new code or documentation to cc65. We are well aware that not all existing code may respect all rules outlined here - but this is no reason for you not to respect them.
* One commit/patch/PR per issue. Do not mix several things unless they are very closely related.

# Codestyle rules

## All Sources

### TABs and spaces

This is an ongoing controversial topic - everyone knows that. However, the following is how we do it :)

* TAB characters must be expanded to spaces.
* 4 spaces per indention level (rather than 8) are preferred, especially if there are many different levels.
* No extra spaces at the end of lines.
* All text files must end with new-line characters.  Don't leave the last line "dangling".

The (bash) scripts used to check the above rules can be found in ```.github/check```. You can also run all checks using ```make check```.

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
* If a function is declared to return a char-sized value, it actually must return an integer-sized value.  (When cc65 promotes a returned value, it sometimes assumes that the value already is an integer.)
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

## Test suite

* specific tests to check the optimizer (rather than the codegenerator) are needed.
* we need more specific tests to check standard conformance of the library headers
