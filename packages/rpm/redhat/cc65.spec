#
# SPEC file for the cc65 C compiler and RedHat 7.x
#
# 03/08/2001, Ullrich von Bassewitz (uz@cc65.org)
#

###############################################################################
#				 Main package				      #
###############################################################################

Summary: Crosscompiler/Crossassembler for 6502 systems
%define name cc65
Name: %{name}
%define version 2.9.5
Version: %{version}
Release: 1
Copyright: Freeware with exceptions - see docs
Group: Development/Languages
Source: ftp://ftp.musoftware.de/uz/cc65/%{name}-sources-%{version}.tar.bz2
Vendor: MU Softwareentwicklung
Packager: Ullrich von Bassewitz <uz@musoftware.de>
%ifos linux
BuildRequires: perl
BuildRequires: sgml-tools
%endif
BuildRoot: /var/tmp/%{name}-%{version}

%description
A C crosscompiler for 6502 systems, including a macroassembler that
supports 6502, 65SC02 and 65816 CPUs, a linker, an archiver and some
other tools. To create programs for one of the supported target
machines, you have to install at least one of the library packages.

%files
%defattr(644,root,root,755)
%doc announce.txt
%doc doc/BUGS
%doc doc/CREDITS
%doc doc/readme.1st
%attr(755,root,root)   		/usr/bin/ar65
%attr(755,root,root)   		/usr/bin/ca65
%attr(755,root,root) 		/usr/bin/ca65html
%attr(755,root,root)   		/usr/bin/cc65
%attr(755,root,root)   		/usr/bin/cl65
%attr(755,root,root)   		/usr/bin/co65
%attr(755,root,root) 		/usr/bin/da65
%attr(755,root,root)   		/usr/bin/ld65
%attr(755,root,root)   		/usr/bin/od65
%attr(755,root,root) %dir	/usr/lib/cc65
%attr(755,root,root) %dir	/usr/lib/cc65/emd
%attr(755,root,root) %dir	/usr/lib/cc65/joy
%attr(755,root,root) %dir	/usr/lib/cc65/lib
%attr(755,root,root) %dir	/usr/lib/cc65/tgi
%attr(755,root,root) %dir	/usr/lib/cc65/include
%attr(644,root,root)   		/usr/lib/cc65/include/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/em
%attr(644,root,root)   		/usr/lib/cc65/include/em/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/joystick
%attr(644,root,root)   		/usr/lib/cc65/include/joystick/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/sys
%attr(644,root,root)   		/usr/lib/cc65/include/sys/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/tgi
%attr(644,root,root)   		/usr/lib/cc65/include/tgi/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/asminc
%attr(644,root,root)   		/usr/lib/cc65/asminc/*.inc



###############################################################################
#                          Documentation subpackage                           #
###############################################################################

%package doc

Requires: %{name} = %{version}
Copyright: Freeware
Summary: Documentation for the cc65 C compiler
Group: Development/Languages

%description doc
This package contains the documentation for the cc65 C compiler, mostly
in HTML format. Other formats can be generated from the sources.

%files doc
%doc doc/*.html
%doc doc/compile.txt
%doc doc/grc.txt
%doc doc/internal.txt
%doc doc/newvers.txt
%doc samples
%doc src/ld65/cfg/*.cfg



###############################################################################
#		     	      Apple ][ subpackage     			      #
###############################################################################

%package apple2
Requires: %{name} = %{version}
Copyright: Freeware
Summary: Apple ][ specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description apple2
This package contains the header files and libraries needed to write
programs for the Apple ][ using the cc65 crosscompiler.

%files apple2
%attr(644,root,root) 		/usr/lib/cc65/lib/apple2.lib
%attr(644,root,root) 		/usr/lib/cc65/lib/apple2.o
%attr(644,root,root) 		/usr/lib/cc65/joy/a2-*.joy
%attr(644,root,root) 		/usr/lib/cc65/tgi/a2-*.tgi



###############################################################################
#	   	      	       Atari subpackage	 	    		      #
###############################################################################

%package atari
Requires: %{name} = %{version}
Copyright: Freeware
Summary: Atari specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description atari
This package contains the header files and libraries needed to write
programs for the 8 bit Atari machines using the cc65 crosscompiler.

%files atari
%attr(644,root,root)  		/usr/lib/cc65/lib/atari.lib
%attr(644,root,root)  		/usr/lib/cc65/lib/atari.o



###############################################################################
#                            Oric Atmos subpackage                            #
###############################################################################

%package atmos
Requires: %{name} = %{version}
Copyright: Freeware
Summary: Oric Atmos specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description atmos
This package contains the header files and libraries needed to write
programs for the Oric Atmos using the cc65 crosscompiler.

%files atmos
%attr(644,root,root)		/usr/lib/cc65/lib/atmos.lib
%attr(644,root,root)		/usr/lib/cc65/lib/atmos.o



###############################################################################
#		     		C128 subpackage	 			      #
###############################################################################

%package c128
Requires: %{name} = %{version}
Copyright: Freeware
Summary: C128 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c128
This package contains the header files and libraries needed to write
programs for the Commodore C128 using the cc65 crosscompiler.

%files c128
%attr(644,root,root) 		/usr/lib/cc65/lib/c128.lib
%attr(644,root,root) 		/usr/lib/cc65/lib/c128.o
%attr(644,root,root) 		/usr/lib/cc65/emd/c128-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/c128-*.joy
%attr(644,root,root) 		/usr/lib/cc65/ser/c128-*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/c128-*.tgi



###############################################################################
#      	       	       	       	 C16 subpackage		   		      #
###############################################################################

%package c16
Requires: %{name} = %{version}
Copyright: Freeware
Summary: C16/116 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c16
This package contains the header files and libraries needed to write
programs for the Commodore C16/116 using the cc65 crosscompiler.

%files c16
%attr(644,root,root) 		/usr/lib/cc65/lib/c16.lib
%attr(644,root,root) 		/usr/lib/cc65/lib/c16.o
%attr(644,root,root) 		/usr/lib/cc65/joy/c16-*.joy



###############################################################################
#		     		C64 subpackage	 	   		      #
###############################################################################

%package c64
Requires: %{name} = %{version}
Copyright: Freeware
Summary: C64 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c64
This package contains the header files and libraries needed to write
programs for the Commodore C64 using the cc65 crosscompiler.

%files c64
%attr(644,root,root) 		/usr/lib/cc65/lib/c64.lib
%attr(644,root,root) 		/usr/lib/cc65/lib/c64.o
%attr(644,root,root) 		/usr/lib/cc65/emd/c64-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/c64-*.joy
%attr(644,root,root) 		/usr/lib/cc65/tgi/c64-*.tgi



###############################################################################
#		       	       CBM510 subpackage       			      #
###############################################################################

%package cbm510
Requires: %{name} = %{version}
Copyright: Freeware
Summary: CBM 510 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description cbm510
This package contains the header files and libraries needed to write
programs for the Commodore CBM 510 (aka P500) using the cc65
crosscompiler.

%files cbm510
%attr(644,root,root)   		/usr/lib/cc65/lib/cbm510.lib
%attr(644,root,root)   		/usr/lib/cc65/lib/cbm510.o
%attr(644,root,root) 		/usr/lib/cc65/emd/cbm510-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/cbm510-*.joy



###############################################################################
#		       	       CBM610 subpackage       			      #
###############################################################################

%package cbm610
Requires: %{name} = %{version}
Copyright: Freeware
Summary: CBM 610 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description cbm610
This package contains the header files and libraries needed to write
programs for the Commodore PET-II (CBM600/700) family of computers using
the cc65 crosscompiler.

%files cbm610
%attr(644,root,root)   		/usr/lib/cc65/lib/cbm610.lib
%attr(644,root,root)   		/usr/lib/cc65/lib/cbm610.o
%attr(644,root,root) 		/usr/lib/cc65/emd/cbm610-*.emd



###############################################################################
#		 		GEOS subpackage	 			      #
###############################################################################

%package geos
Requires: %{name} = %{version}
Copyright: Freeware
Summary: GEOS specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description geos
This package contains the header files and libraries needed to write
GEOS programs for the C64/C128 using the cc65 crosscompiler.

%files geos
%attr(755,root,root)   		/usr/bin/grc
%attr(644,root,root)		/usr/lib/cc65/lib/geos.lib
%attr(644,root,root)		/usr/lib/cc65/lib/geos.o
%attr(755,root,root) %dir	/usr/lib/cc65/include/geos
%attr(644,root,root)   		/usr/lib/cc65/include/geos/*.h
%attr(644,root,root) 		/usr/lib/cc65/emd/geos-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/geos-*.joy
%attr(644,root,root) 		/usr/lib/cc65/tgi/geos-*.tgi



###############################################################################
#		       		NES subpackage	       		       	      #
###############################################################################

%package nes
Requires: %{name} = %{version}
Copyright: Freeware
Summary: NES specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description nes
This package contains the header files and libraries needed to write
programs for the Nintendo Entertainment System using the cc65
crosscompiler.

%files nes
%attr(644,root,root)		/usr/lib/cc65/lib/nes.lib
%attr(644,root,root)		/usr/lib/cc65/lib/nes.o



###############################################################################
#		       		PET subpackage	       		       	      #
###############################################################################

%package pet
Requires: %{name} = %{version}
Copyright: Freeware
Summary: PET specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description pet
This package contains the header files and libraries needed to write
programs for the Commodore PET family of computers using the cc65
crosscompiler.

%files pet
%attr(644,root,root)		/usr/lib/cc65/lib/pet.lib
%attr(644,root,root)		/usr/lib/cc65/lib/pet.o



###############################################################################
#		       	       Plus/4 subpackage 	    		      #
###############################################################################

%package plus4
Requires: %{name} = %{version}
Copyright: Freeware
Summary: Plus/4 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description plus4
This package contains the header files and libraries needed to write
programs for the Commodore Plus/4 and C16/116 using the cc65
crosscompiler.

%files plus4
%attr(644,root,root)   		/usr/lib/cc65/lib/plus4.lib
%attr(644,root,root)   		/usr/lib/cc65/lib/plus4.o
%attr(644,root,root) 		/usr/lib/cc65/joy/plus4-*.joy



###############################################################################
#                           Supervision subpackage                            #
###############################################################################

%package supervision
Requires: %{name} = %{version}
Copyright: Freeware
Summary: Supervision specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description supervision
This package contains the header files and libraries needed to write
programs for the Supervision Game console using the cc65 crosscompiler.

%files supervision
%attr(644,root,root)		/usr/lib/cc65/lib/supervision.lib
%attr(644,root,root)		/usr/lib/cc65/lib/supervision.o



###############################################################################
#		     	       VIC20 subpackage	  			      #
###############################################################################

%package vic20
Requires: %{name} = %{version}
Copyright: Freeware
Summary: VIC20 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description vic20
This package contains the header files and libraries needed to write
programs for the Commodore VIC20 using the cc65 crosscompiler.

%files vic20
%attr(644,root,root) 		/usr/lib/cc65/lib/vic20.lib
%attr(644,root,root) 		/usr/lib/cc65/lib/vic20.o
%attr(644,root,root) 		/usr/lib/cc65/joy/vic20-*.joy



###############################################################################
#		      		     Build	 	    		      #
###############################################################################



%prep
%setup -q


%build
# Binaries
make -C src -f make/gcc.mak
# Libraries
make -C libsrc -s zap all
# Documentation
make -C doc html


%install
mkdir -p $RPM_BUILD_ROOT/usr/{bin,lib/cc65/{asminc,emd,include/{em,geos,joystick,sys,tgi},joy,lib,tgi}}

# Binaries
install -s -m 755 src/ar65/ar65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ca65/ca65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ca65html/ca65html $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cc65/cc65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cl65/cl65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/co65/co65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/da65/da65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/grc/grc $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ld65/ld65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/od65/od65 $RPM_BUILD_ROOT/usr/bin
# Libraries and includes
install -m 644 libsrc/*.lib libsrc/*.o $RPM_BUILD_ROOT/usr/lib/cc65/lib
install -m 644 include/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include
install -m 644 include/geos/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/geos
install -m 644 include/em/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/em
install -m 644 include/joystick/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/joystick
install -m 644 include/sys/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/sys
install -m 644 include/tgi/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/tgi
install -m 644 asminc/*.inc $RPM_BUILD_ROOT/usr/lib/cc65/asminc

# Drivers
install -m 644 libsrc/*.emd $RPM_BUILD_ROOT/usr/lib/cc65/emd
install -m 644 libsrc/*.joy $RPM_BUILD_ROOT/usr/lib/cc65/joy
install -m 644 libsrc/*.ser $RPM_BUILD_ROOT/usr/lib/cc65/ser
install -m 644 libsrc/*.tgi $RPM_BUILD_ROOT/usr/lib/cc65/tgi


%clean
rm -rf $RPM_BUILD_ROOT



%changelog
* Sun Nov 02 2003 Ullrich von Bassewitz <uz@cc65.org>
- Added missing files after upgrade to rpm 4.2

* Sun Oct 19 2003 Ullrich von Bassewitz <uz@cc65.org>
- Move documentation into a separate doc package.

* Fri Oct 10 2003 Ullrich von Bassewitz <uz@cc65.org>
- Added the Supervision subpackage.

* Thu Oct 02 2003 Ullrich von Bassewitz <uz@cc65.org>
- Better cleanup.

* Thu Jun 12 2003 Ullrich von Bassewitz <uz@cc65.org>
- Added subdirectories of include/

* Fri May 02 2003 Ullrich von Bassewitz <uz@cc65.org>
- The Apple ][ has a joystick driver.
- Added the NES subpackage.

* Mon Apr 14 2003 Ullrich von Bassewitz <uz@cc65.org>
Added a subpackage for the Oric Atmos.

* Mon Feb 10 2003 Ullrich von Bassewitz <uz@cc65.org>
Added the object file conversion utility co65.

* Fri Dec 13 2002 Ullrich von Bassewitz <uz@cc65.org>
Updated the version number.

* Mon Dec 2 2002 Ullrich von Bassewitz <uz@cc65.org>
Changed location of TGI drivers, added EM drivers.

* Fri Nov 22 2002 Ullrich von Bassewitz <uz@cc65.org>
Added the C16 subpackage.

* Wed Nov 20 2002 Ullrich von Bassewitz <uz@cc65.org>
Added TGI stuff.

* Mon Apr 08 2002 Ullrich von Bassewitz <uz@cc65.org>
Update to version 2.8.0 of the compiler package.

* Wed Nov 07 2001 Ullrich von Bassewitz <uz@cc65.org>
Update to version 2.7.1 of the compiler package.

* Tue Oct 02 2001 Ullrich von Bassewitz <uz@cc65.org>
Update to version 2.7.0 of the compiler package.

* Thu Jul 23 2001 Ullrich von Bassewitz <uz@cc65.org>
Update to version 2.6.2 of the compiler package.

* Thu Mar 8 2001 Ullrich von Bassewitz <uz@cc65.org>
Update to version 2.6.1 of the compiler package.

* Sat Dec 6 2000 Ullrich von Bassewitz <uz@cc65.org>
Update for cc65 2.6.0 and RedHat 7.x

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

