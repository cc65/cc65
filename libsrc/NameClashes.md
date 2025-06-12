List of cc65 library name clashes
=================================

The following is a list of identifiers that might need
to be fixed, sorted by directory and identifier:

# common

## \_\_argc

* libsrc/runtime/callmain.s
* libsrc/cbm610/mainargs.s
* libsrc/cx16/mainargs.s
* libsrc/plus4/mainargs.s
* libsrc/lynx/mainargs.s
* libsrc/c16/mainargs.s
* libsrc/geos-common/system/mainargs.s
* libsrc/sim6502/mainargs.s
* libsrc/c128/mainargs.s
* libsrc/vic20/mainargs.s
* libsrc/nes/mainargs.s
* libsrc/atari/getargs.s
* libsrc/apple2/mainargs.s
* libsrc/cbm510/mainargs.s
* libsrc/telestrat/mainargs.s
* libsrc/c64/mainargs.s
* libsrc/pet/mainargs.s
* libsrc/atmos/mainargs.s

## \_\_argv

* libsrc/runtime/callmain.s
* libsrc/cbm610/mainargs.s
* libsrc/cx16/mainargs.s
* libsrc/plus4/mainargs.s
* libsrc/lynx/mainargs.s
* libsrc/c16/mainargs.s
* libsrc/geos-common/system/mainargs.s
* libsrc/sim6502/mainargs.s
* libsrc/c128/mainargs.s
* libsrc/vic20/mainargs.s
* libsrc/nes/mainargs.s
* libsrc/atari/getargs.s
* libsrc/apple2/mainargs.s
* libsrc/cbm510/mainargs.s
* libsrc/telestrat/mainargs.s
* libsrc/c64/mainargs.s
* libsrc/pet/mainargs.s
* libsrc/atmos/mainargs.s

## \_\_cos

* libsrc/common/sincos.s

## \_\_ctypeidx

* libsrc/common/ctype.s
* libsrc/common/ctypemask.s
* libsrc/geos-common/system/ctype.s
* libsrc/atari/ctype.s
* libsrc/cbm/ctype.s
* libsrc/atmos/ctype.s
* asminc/ctype\_common.inc

## \_\_cwd

* libsrc/common/getcwd.s
* libsrc/common/_cwd.s
* libsrc/atari/initcwd.s
* libsrc/apple2/initcwd.s
* libsrc/apple2/initcwd.s
* libsrc/telestrat/initcwd.s
* libsrc/cbm/initcwd.s

## \_\_cwd\_buf\_size

* libsrc/common/_cwd.s

## \_\_envcount

* libsrc/common/searchenv.s
* libsrc/common/_environ.s
* libsrc/common/putenv.s
* libsrc/common/getenv.s

## \_\_environ

* libsrc/common/searchenv.s
* libsrc/common/_environ.s
* libsrc/common/putenv.s
* libsrc/common/getenv.s

## \_\_envsize

* libsrc/common/_environ.s
* libsrc/common/putenv.s

## \_\_fdesc

* libsrc/common/_fdesc.s
* libsrc/common/fopen.s

## \_\_filetab

* libsrc/common/_fdesc.s
* libsrc/common/_file.s
* asminc/_file.inc

## \_\_fopen

* libsrc/common/fopen.s
* libsrc/common/_fopen.s

## \_\_printf

* libsrc/common/vsnprintf.s
* libsrc/common/_printf.s
* libsrc/common/vfprintf.s
* libsrc/conio/vcprintf.s
* libsrc/pce/_printf.s

## \_\_scanf

* libsrc/common/_scanf.inc
* libsrc/common/vsscanf.s
* libsrc/conio/vcscanf.s

## \_\_sin

* libsrc/common/sincos.s

## \_\_sys

* libsrc/common/_sys.s
* libsrc/apple2/_sys.s

## \_\_sys\_oserrlist

* libsrc/common/stroserr.s
* libsrc/geos-common/system/oserrlist.s
* libsrc/atari/oserrlist.s
* libsrc/apple2/oserrlist.s
* libsrc/cbm/oserrlist.s
* libsrc/atmos/oserrlist.s

## \_\_syschdir

* libsrc/common/chdir.s
* libsrc/atari/syschdir.s
* libsrc/apple2/syschdir.s
* libsrc/telestrat/syschdir.s
* libsrc/cbm/syschdir.s

## \_\_sysmkdir

* libsrc/common/mkdir.s
* libsrc/atari/sysmkdir.s
* libsrc/apple2/sysmkdir.s
* libsrc/telestrat/sysmkdir.s

## \_\_sysremove

* libsrc/common/remove.s
* libsrc/geos-common/file/sysremove.s
* libsrc/atari/sysremove.s
* libsrc/atari/sysrmdir.s
* libsrc/apple2/sysremove.s
* libsrc/apple2/sysrmdir.s
* libsrc/telestrat/sysremove.s
* libsrc/cbm/sysremove.s

## \_\_sysrename

* libsrc/common/rename.s
* libsrc/geos-common/file/sysrename.s
* libsrc/atari/sysrename.s
* libsrc/apple2/sysrename.s
* libsrc/cbm/sysrename.s

## \_\_sysrmdir

* libsrc/common/rmdir.s
* libsrc/atari/sysrmdir.s
* libsrc/apple2/sysrmdir.s

\_\_sysuname

* libsrc/common/uname.s
* libsrc/cbm610/sysuname.s
* libsrc/cx16/sysuname.s
* libsrc/plus4/sysuname.s
* libsrc/lynx/sysuname.s
* libsrc/c16/sysuname.s
* libsrc/geos-common/system/sysuname.s
* libsrc/c128/sysuname.s
* libsrc/creativision/sysuname.s
* libsrc/vic20/sysuname.s
* libsrc/nes/sysuname.s
* libsrc/atari/sysuname.s
* libsrc/apple2/sysuname.s
* libsrc/cbm510/sysuname.s
* libsrc/telestrat/sysuname.s
* libsrc/c64/sysuname.s
* libsrc/pet/sysuname.s
* libsrc/atari5200/sysuname.s
* libsrc/atmos/sysuname.s

# apple2

## \_\_auxtype

* libsrc/apple2/open.s

## \_\_datetime

* libsrc/apple2/open.s

## \_\_dos\_type

* libsrc/apple2/dioopen.s
* libsrc/apple2/curdevice.s
* libsrc/apple2/mainargs.s
* libsrc/apple2/settime.s
* libsrc/apple2/getdevice.s
* libsrc/apple2/dosdetect.s
* libsrc/apple2/irq.s
* libsrc/apple2/open.s
* libsrc/apple2/mli.s
* libsrc/apple2/getres.s

## \_\_filetype

* libsrc/apple2/open.s
* libsrc/apple2/exehdr.s

## atari

## \_\_defdev

* libsrc/atari/posixdirent.s
* libsrc/atari/ucase\_fn.s
* libsrc/atari/getdefdev.s

## \_\_dos\_type

* libsrc/atari/getargs.s
* libsrc/atari/exec.s
* libsrc/atari/settime.s
* libsrc/atari/syschdir.s
* libsrc/atari/dosdetect.s
* libsrc/atari/is\_cmdline\_dos.s
* libsrc/atari/sysrmdir.s
* libsrc/atari/gettime.s
* libsrc/atari/lseek.s
* libsrc/atari/getres.s
* libsrc/atari/getdefdev.s

## \_\_do\_oserror

* libsrc/atari/posixdirent.s
* libsrc/atari/do\_oserr.s
* libsrc/atari/serref.s
* libsrc/atari/read.s
* libsrc/atari/write.s
* libsrc/atari/close.s

## \_\_getcolor

* libsrc/atari/setcolor.s

## \_\_getdefdev

* libsrc/atari/getdefdev.s

## \_\_graphics

* libsrc/atari/graphics.s

## \_\_inviocb

* libsrc/atari/serref.s
* libsrc/atari/ser/atrrdev.s
* libsrc/atari/inviocb.s
* libsrc/atari/read.s
* libsrc/atari/write.s
* libsrc/atari/lseek.s
* libsrc/atari/close.s

## \_\_is\_cmdline\_dos

* libsrc/atari/is\_cmdline\_dos.s
* libsrc/atari/doesclrscr.s

## \_\_rest\_vecs

* libsrc/atari/savevec.s

## \_\_rwsetup

* libsrc/atari/rwcommon.s
* libsrc/atari/read.s
* libsrc/atari/write.s

## \_\_save\_vecs

* libsrc/atari/savevec.s

## \_\_scroll

* libsrc/atari/scroll.s

## \_\_setcolor

* libsrc/atari/setcolor.s

## \_\_setcolor\_low

* libsrc/atari/setcolor.s

## \_\_sio\_call

* libsrc/atari/diowritev.s
* libsrc/atari/diopncls.s
* libsrc/atari/siocall.s
* libsrc/atari/diowrite.s
* libsrc/atari/dioread.s

# cbm

## \_\_cbm\_filetype

* libsrc/cbm/cbm\_filetype.s
* asminc/cbm\_filetype.in

## \_\_dirread

* libsrc/cbm/dir.inc
* libsrc/cbm/dir.s

## \_\_dirread1

* libsrc/cbm/dir.inc
* libsrc/cbm/dir.s

# lynx

## \_\_iodat

* libsrc/lynx/lynx-cart.s
* libsrc/lynx/bootldr.s
* libsrc/lynx/extzp.s
* libsrc/lynx/crt0.s
* libsrc/lynx/extzp.inc

## \_\_iodir

* libsrc/lynx/extzp.s
* libsrc/lynx/crt0.s
* libsrc/lynx/extzp.inc

## \_\_sprsys

* libsrc/lynx/tgi/lynx-160-102-16.s
* libsrc/lynx/extzp.s
* libsrc/lynx/crt0.s
* libsrc/lynx/extzp.inc

## \_\_viddma

* libsrc/lynx/tgi/lynx-160-102-16.s
* libsrc/lynx/extzp.s
* libsrc/lynx/crt0.s
* libsrc/lynx/extzp.inc

# pce

## \_\_nmi

* libsrc/pce/irq.s
* libsrc/pce/crt0.s
