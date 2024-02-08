
This directory contains sample programs for the cc65 compiler.

The programs contained here are
- used as samples and referenced from within the documentation (this is their
  main purpose)
- installed into the filesystem
- compiled (but NOT run) by the CI test

-----------------------------------------------------------------------------

Below is a short description for each of the programs, together with a list
of the supported platforms.

Please note:

  * The supplied makefile needs GNU make. It works out of the box on Linux and
    similar systems. If you're using Windows, then consider installing Cygwin
    or MSys2.

  * The makefile specifies the C64 as the default target system because most
    of the programs run on that platform. When compiling for another system,
    you will have to change the line that specifies the target system at the
    top of the makefile, specify the system with SYS=<target> on the make
    command line, or set a SYS environment variable. For example:

    make SYS=apple2

  * Use "make disk" to build a disk image with all sample programs.

  * All programs in the root of the "samples" directory have been written to
    be portable and work on more than one target. Programs that are specific
    to a certain target live in a subdirectory with the name of the target.

List of supplied sample programs:

-----------------------------------------------------------------------------
Name:           ascii
Description:    Shows the ASCII (or ATASCII, PETSCII) codes of typed
                characters. Written and contributed by Greg King
                <greg.king5@verizon.com>.
Platforms:      All platforms with conio or stdio (compile time configurable).

-----------------------------------------------------------------------------
Name:           diodemo
Description:    A disc copy program written and contributed by Oliver
                Schmidt, <ol.sc@web.de>. Supports single or dual disc copy.
Platforms:      The program does depend on conio and dio (direct disk i/o),
                so it currently does compile for only the Atari and Apple ][
                machines.

-----------------------------------------------------------------------------
Name:           enumdevdir
Description:    Enumerates all devices, directories, and files. Written and
                contributed by Oliver Schmidt, <ol.sc@web.de>.
Platforms:      All systems with device enumeration and directory access
                (currently the Commodore machines, the Commander X16,
                and the Apple ][).

-----------------------------------------------------------------------------
Name:           gunzip65
Description:    A gunzip utility for 6502-based machines, written by Piotr
                Fusik <fox@scene.pl>.
Platforms:      Runs on all platforms with file I/O (currently the Atari, the
                Apple ][, Commodore machines, and the Commander X16).

-----------------------------------------------------------------------------
Name:           hello
Description:    A nice "Hello world" type program that uses the conio
                console I/O library for output.
Platforms:      Runs on all platforms that support conio, which means:
                Apple ][, Atari, Commodore machines, Commander X16,
                Creativision, Gamate, NES.

-----------------------------------------------------------------------------
Name:           mandelbrot
Description:    A mandelbrot demo using integer arithmetic. The demo was
                written by groepaz, and converted to cc65 using TGI graphics
                by Stefan Haubenthal.
Platforms:      Runs on all platforms that have TGI support:
                Apple ][, Atari, C64, C128, Oric Atmos and Telestrat, GEOS,
                and NES.

-----------------------------------------------------------------------------
Name:           mousedemo
Description:    Shows how to use the mouse.
Platforms:      All systems with mouse and conio support:
                C64, C128, CBM510, Atari, Apple ][.

-----------------------------------------------------------------------------
Name:           multidemo
Description:    Shows how to combine multiple cc65 features incl. overlays
                and extended memory drivers. Written and contributed by
                Oliver Schmidt, <ol.sc@web.de>.
Platforms:      All systems with an overlay linker config., disk directory
                access, and EMD support (currently the C64, the C128,
                the Atari, and the Apple ][).

-----------------------------------------------------------------------------
Name:           overlaydemo
Description:    Shows how to load overlay files from disk. Written and
                contributed by Oliver Schmidt, <ol.sc@web.de>.
Platforms:      All systems with an overlay linker config. (currently the C64,
                the C128, the Atari, and the Apple ][).

-----------------------------------------------------------------------------
Name:           sieve
Description:    Implements the "Sieve of Eratosthenes" as a way to find all
                prime numbers in a specific number interval. Often used as
                a benchmark program.
Platforms:      All systems with conio and clock support:
                Atari, Commodore machines (VIC-20 needs memory expansion),
                Commander X16, Apple ][ (without timing due to missing clock
                support).

-----------------------------------------------------------------------------
Name:           tinyshell
Description:    Simple ("tiny") shell to test filename and directory functions.
Platforms:      Runs on all platforms that have stdio support

-----------------------------------------------------------------------------
Name:           tgidemo
Description:    Shows some of the graphics capabilities of the "Tiny Graphics
                Interface".
Platforms:      Runs on all platforms that have TGI support:
                Apple ][, Atari, C64, C128, Oric Atmos and Telestrat, GEOS,
                NES, and Lynx.

=============================================================================

Platform-specific samples follow:

atari 2600:
-----------

Name:           hello
Description:    A "Hello world" type program.
Platforms:      Runs on only the Atari 2600 Video Console System.
-----------------------------------------------------------------------------

atari 5200:
-----------

Name:           hello
Description:    A "Hello world" type program.
Platforms:      Runs on only the Atari 5200 Video Console System.
-----------------------------------------------------------------------------

apple2:
-------

Name:           hgrtest 
Description:    shows how to incorporate a hires screen right into the binary,
                thus allowing to place content both below and above the hires
                video ram without multiple files nor copying stuff around (plus
                having a nice splash screen as bonus). It basically demos how to
                make use of apple2-hgr.cfg.

Name:           hgrshow 
Description:    is a (single) hires program showing how to load (single) hires
                screens from files without support from TGI. TGI could be used
                to "paint" on the loaded screens.

Name:           dhgrshow 
Description:    is a double hires program showing how to load double hires
                screens from files. TGI is used for most of the I/O address
                manipulation, but (in contrast to hgrshow) is not actually
                functional.

-----------------------------------------------------------------------------

cbm:
----

Name:           fire
Description:    Another graphics demo written by groepaz.
Platforms:      C64, C128, CBM510

-----------------------------------------------------------------------------
Name:           nachtm
Description:    Plays "Eine kleine Nachtmusik" by Wolfgang Amadeus Mozart.
Platforms:      All systems that have the Commodore SID (Sound Interface
                Device):
                C64, C128, CBM510, CBM610.

-----------------------------------------------------------------------------
Name:           plasma
Description:    A fancy graphics demo written by groepaz.
Platforms:      The program needs a VIC-II or a TED, so it runs on the following
                systems:
                C64, C128, CBM510, Plus/4.
-----------------------------------------------------------------------------

gamate:
-------

Name:           nachtm
Description:    Plays "Eine kleine Nachtmusik" by Wolfgang Amadeus Mozart.
-----------------------------------------------------------------------------

lynx:
-----

These programs are adapted for the Atari Lynx because its library has no conio
output or stdio.

Name:           hello
Description:    A nice "Hello world" type program that uses the TGI graphics
                library for output.

Name:           mandelbrot
Description:    A mandelbrot demo using integer arithmetic. The demo was
                written by groepaz, and converted to cc65 using TGI graphics
                by Stefan Haubenthal.

Name:           tgidemo
Description:    Shows some of the graphics capabilities of the "Tiny Graphics
                Interface".

-----------------------------------------------------------------------------

sym1:
-----

Name:           symHello
Description:    Hello World for Sym-1

Name:           symTiny
Description:    Hello World for Sym-1 (tiny version without printf)

Name:           symDisplay
Description:    Sym-1 front panel display example

Name:           symIO
Description:    Sym-1 digital I/O interface example

Name:           symNotepad
Description:    Sym-1 Notepad

-----------------------------------------------------------------------------

supervision:
------------

Name:           hello
Description:    A "Hello world" type program.
Platforms:      Runs on only the Watara Supervision game console.
----------------------------------------------------------------------------
