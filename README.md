The cc65 cross-compiler suite
=============================

cc65 is a complete cross-development package for 65(C)02 systems,
including a powerful macro assembler, a C compiler, linker, archiver,
simulator and several other tools.  cc65 has C and runtime library
support for many of the old 6502 machines.  For details look at
the [cc65 web site](https://cc65.github.io):

| Company / People        | Machine / Environment               |
|-------------------------|-------------------------------------|
| Apple                   | Apple II                            |
|                         | Apple IIe enhanced                  |
| Atari                   | Atari 400/800                       |
|                         | Atari 2600                          |
|                         | Atari 5200                          |
|                         | Atari 7800                          |
|                         | Atari XL                            |
|                         | Lynx                                |
| Tangerine               | Oric Atmos                          |
| Eureka                  | Oric Telestrat                      |
| Acorn                   | BBC series                          |
| Commodore               | C128                                |
|                         | C16                                 |
|                         | C64                                 |
|                         | CBM 510/610                         |
|                         | PET                                 |
|                         | Plus/4                              |
|                         | VIC-20                              |
| VTech                   | CreatiVision                        |
| Commander X16 Community | Commander X16                       |
| Bit Corporation         | Gamate                              |
| Berkeley Softworks      | GEOS (Apple/CBM)                    |
| LUnix Team              | LUnix (C64)                         |
| Nintendo                | Nintendo Entertainment System (NES) |
| Ohio Scientific         | OSI C1P                             |
| MOS Technology, Inc.    | KIM-1                               |
| NEC                     | PC Engine (PCE)                     |
| Dr. Jozo Dujmović       | Picocomputer (RP6502)               |
| Watara                  | Watura/QuickShot Supervision        |
| Synertek                | SYM-1                               |

A generic configuration to adapt cc65 to new targets is also around.

## People

cc65 is originally based on the "Small C" compiler by Ron Cain and
enhanced by James E. Hendrix.

### Project founders

* John R. Dunning: [original implementation](https://public.websites.umich.edu/~archive/atari/8bit/Languages/Cc65/)
  of the C compiler and runtime library, Atari hosted.
* Ullrich von Bassewitz:
  * moved Dunning's code to modern systems,
  * rewrote most parts of the compiler,
  * rewrote all of the runtime library.

### Core team members

* [Christian Groessler](https://github.com/groessler): Atari, Atari5200, and CreatiVision library Maintainer
* [dqh](https://github.com/dqh-au): GHA help
* [Greg King](https://github.com/greg-king5): all around hackery
* [groepaz](https://github.com/mrdudz): CBM library, Project Maintainer
* [Oliver Schmidt](https://github.com/oliverschmidt): Apple II library Maintainer

### External contributors

* [acqn](https://github.com/acqn): various compiler fixes
* [jedeoric](https://github.com/jedeoric): Telestrat target
* [jmr](https://github.com/jmr): compiler fixes
* [karrika](https://github.com/karrika): Atari 7800 target
* [Stephan Mühlstrasser](https://github.com/smuehlst): osic1p target
* [Wayne Parham](https://github.com/WayneParham): Sym-1 target
* [Dave Plummer](https://github.com/davepl): KIM-1 target
* [rumbledethumps](https://github.com/rumbledethumps): Picocomputer target

*(The above list is incomplete, if you feel left out - please speak up or add yourself in a PR)*

For a complete list look at the [full team list](https://github.com/orgs/cc65/teams)
or the list of [all contributors](https://github.com/cc65/cc65/graphs/contributors).

# Contact

For general discussion, questions, etc subscribe to the
[mailing list](https://cc65.github.io/mailing-lists.html)
or use the [github discussions](https://github.com/cc65/cc65/discussions).

Some of us may also be around on IRC [#cc65](https://web.libera.chat/#cc65) on libera.chat.

# Documentation

* The main [Documentation](https://cc65.github.io/doc) for users and
  developers.
* Info on [Contributing](Contributing.md) to the CC65 project. Please
  read this before working on something you want to contribute, and
  before reporting bugs.
* The [Wiki](https://github.com/cc65/wiki/wiki) contains some extra info
  that does not fit into the regular documentation.

# Downloads

* [Windows 64bit Snapshot](https://sourceforge.net/projects/cc65/files/cc65-snapshot-win64.zip)
* [Windows 32bit Snapshot](https://sourceforge.net/projects/cc65/files/cc65-snapshot-win32.zip)
* [Linux Snapshot DEB and RPM](https://software.opensuse.org/download.html?project=home%3Astrik&package=cc65)

[![Snapshot Build](https://github.com/cc65/cc65/actions/workflows/snapshot-on-push-master.yml/badge.svg?branch=master)](https://github.com/cc65/cc65/actions/workflows/snapshot-on-push-master.yml)
