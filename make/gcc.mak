#
# Main gcc Makefile
#

# Install prefix and directories
PREFIX	= /usr
BINDIR	= $(PREFIX)/bin
LIBDIR	= $(PREFIX)/lib
DOCDIR	= $(PREFIX)/share/doc
CC65DATA= $(LIBDIR)/cc65
CC65LIB	= $(CC65DATA)/lib
CC65INC = $(CC65DATA)/include
CC65DOC = $(DOCDIR)/cc65


# Programs
MKDIR	= mkdir
INSTALL	= install


.PHONY:	all
all:	bin libs docs

.PHONY: bin
bin:
	@$(MAKE) -C src -f make/gcc.mak

.PHONY:	libs
libs:
	@$(MAKE) -C libsrc zap all

.PHONY:	docs
docs:
	@$(MAKE) -C doc html

.PHONY: clean
clean:
	$(MAKE) -C src -f make/gcc.mak clean
	$(MAKE) -C libsrc clean
	$(MAKE) -C doc clean

.PHONY: zap
zap:
	$(MAKE) -C src -f make/gcc.mak zap
	$(MAKE) -C libsrc zap
	$(MAKE) -C doc zap

.PHONY:	install
install:	all
	@if [ `id -u` != 0 ]; then				      \
	    echo "";						      \
	    echo 'Do "make install" as root';			      \
	    echo "";						      \
	    false;						      \
	fi
	@$(MKDIR) -p $(BINDIR) $(CC65DOC) $(CC65LIB)/{asminc,emd,include/{em,geos,joystick,sys,tgi},joy,lib,tgi}
	@$(INSTALL) -s -m 755 src/ar65/ar65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/ca65/ca65 $(BINDIR)
	@$(INSTALL)    -m 755 src/ca65html/ca65html $(BINDIR)
	@$(INSTALL) -s -m 755 src/cc65/cc65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/cl65/cl65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/co65/co65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/da65/da65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/grc/grc $(BINDIR)
	@$(INSTALL) -s -m 755 src/ld65/ld65 $(BINDIR)
	@$(INSTALL) -s -m 755 src/od65/od65 $(BINDIR)
	@$(INSTALL) -m 644 libsrc/*.lib libsrc/*.o $(CC65LIB)
	@$(INSTALL) -m 644 include/*.h $(CC65INC)
	@$(INSTALL) -m 644 include/geos/*.h $(CC65INC)/geos
	@$(INSTALL) -m 644 include/em/*.h $(CC65INC)/em
	@$(INSTALL) -m 644 include/joystick/*.h $(CC65INC)/joystick
	@$(INSTALL) -m 644 include/sys/*.h $(CC65INC)/sys
	@$(INSTALL) -m 644 include/tgi/*.h $(CC65INC)/tgi
	@$(INSTALL) -m 644 asminc/*.inc $(CC65DATA)/asminc
	@$(INSTALL) -m 644 libsrc/*.emd $(CC65DATA)/emd
	@$(INSTALL) -m 644 libsrc/*.joy $(CC65DATA)/joy
	@$(INSTALL) -m 644 libsrc/*.tgi $(CC65DATA)/tgi
	@$(INSTALL) -m 644 doc/*.html $(CC65DOC)
	@$(INSTALL) -m 644 doc/{compile.txt,grc.txt,internal.txt,newvers.txt} $(CC65DOC)
	@$(INSTALL) -m 644 src/ld65/cfg/*.cfg $(CC65DOC)

