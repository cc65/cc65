#
# SPEC file for the cc65 C compiler
#
# 07/10/99, Ullrich von Bassewitz (uz@musoftware.de)
#

Summary: Crosscompiler/Crossassembler for 6502 systems
Name: cc65
%define version 2.5.0
Version: %{version}
Release: 3
Copyright: Freeware with exceptions - see docs
Group: Development/Languages
Source: ftp://ftp.musoftware.de/uz/cc65/cc65-sources-%{version}.tar.gz
Vendor: MU Softwareentwicklung
Packager: Ullrich von Bassewitz <uz@musoftware.de>
%ifos linux
BuildRequires: sgml-tools
%endif
BuildRoot: /var/tmp/%{name}-%{version}

%description
A C crosscompiler for 6502 systems, including a macroassembler that
supports 6502, 65SC02 and 65816 CPUs, a linker and an archiver. The
package contains the runtime library, a standard C library and runtime
support for the C64, the C128, the CBM Plus/4, PET machines, the CBM-II
(600/700) series of computers, the Apple ][, and the 8 bit Atari
machines.


%prep
%setup


%build
# Binaries
cd src
make -f make/gcc.mak
cd ..
# Libraries
cd libsrc
make clean apple2lib
make clean atarilib
make clean c128lib
make clean c64lib
make clean cbm610lib
make clean geoslib
make clean petlib
make clean plus4lib
cd ..
# Documentation
cd doc
sgml2txt geos.sgml
cd ..


%install

mkdir -p $RPM_BUILD_ROOT/usr/{bin,lib/cc65/{lib,include/geos,sample}}

# Binaries
install -s -m 755 src/ar65/ar65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ca65/ca65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cc65/cc65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cl65/cl65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/grc/grc $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ld65/ld65 $RPM_BUILD_ROOT/usr/bin
# Libraries and includes
install -m 644 libsrc/*.lib libsrc/*.o $RPM_BUILD_ROOT/usr/lib/cc65/lib
install -m 644 include/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include
install -m 644 include/geos/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/geos


%files
%defattr(644,root,root,755)
%doc announce.txt
%doc doc/BUGS
%doc doc/CREDITS
%doc doc/ar65.txt
%doc doc/ca65.txt
%doc doc/cc65.txt
%doc doc/cl65.txt
%doc doc/coding.txt
%doc doc/compile.txt
%doc doc/debugging.txt
%doc doc/geos.txt
%doc doc/grc.txt
%doc doc/internal.doc
%doc doc/intro.txt
%doc doc/ld65.txt
%doc doc/library.txt
%doc doc/newvers.txt
%doc doc/readme.1st
%doc doc/readme.txt
%doc samples
%attr(755,root,root)   		/usr/bin/ar65
%attr(755,root,root)   		/usr/bin/ca65
%attr(755,root,root)   		/usr/bin/cc65
%attr(755,root,root)   		/usr/bin/cl65
%attr(755,root,root)   		/usr/bin/grc
%attr(755,root,root)   		/usr/bin/ld65
%attr(755,root,root) %dir	/usr/lib/cc65
%attr(755,root,root) %dir	/usr/lib/cc65/lib
%attr(755,root,root) %dir	/usr/lib/cc65/include
%attr(755,root,root) %dir	/usr/lib/cc65/include/geos
%attr(644,root,root)   		/usr/lib/cc65/lib/*
%attr(644,root,root)   		/usr/lib/cc65/include/*.h
%attr(644,root,root)   		/usr/lib/cc65/include/geos/*.h

%changelog
* Sun Jul 30 2000 Ullrich von Bassewitz <uz@musoftware.de>
The GEOS include files were not installed, remove headegen.sh

* Tue Jul 18 2000 Ullrich von Bassewitz <uz@musoftware.de>
Update to version 2.5.0 of the compiler package.

* Sat May 06 2000 Ullrich von Bassewitz <uz@musoftware.de>
Update to version 2.4.1 of the compiler package.

* Tue Jan 16 2000 Ullrich von Bassewitz <uz@musoftware.de>
Update to version 2.4.0 of the compiler package.

* Tue Jan 6 2000 Ullrich von Bassewitz <uz@musoftware.de>
Update to version 2.3.1 of the compiler package.

* Tue Jan 4 2000 Ullrich von Bassewitz <uz@musoftware.de>
Update to version 2.3.0 of the compiler package.

* Sat Sep 25 1999 Robert R. Wal <rrw@reptile.eu.org>
[2.2.1-2]
- changed to allow build as plain user
- `samples' are part of documentation, not part of library

