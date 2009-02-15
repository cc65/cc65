# -*- makefile -*-
#
# Makefile for the cc65 documentation
#

# You can put navigation-arrow pictures (next, back, contents) into HTML files
# by setting this variable, either here or on make's command-line.
# (You will need to copy the .png files from "share/doc/linuxdoc-tools*/html/".)
#
#BUTTONS=-I

# These options decide how text files are made:
# -f -- removes the backspace-overtyping that makes bold text
#       in the more/less commands and on typewriter-printers
# -m -- makes Unix manual pages
#
#TXT_OPTIONS=-f -m

SGML =	apple2.sgml	\
	apple2enh.sgml	\
	ar65.sgml	\
	atari.sgml	\
	atmos.sgml	\
	c128.sgml	\
	c16.sgml	\
	c64.sgml	\
	ca65.sgml	\
	ca65html.sgml	\
	cbm510.sgml	\
	cbm610.sgml	\
	cc65.sgml	\
	cl65.sgml	\
	co65.sgml	\
	coding.sgml	\
	da65.sgml	\
	debugging.sgml	\
	dio.sgml	\
	funcref.sgml	\
	geos.sgml	\
	grc.sgml	\
	index.sgml	\
	intro.sgml	\
	ld65.sgml	\
	library.sgml	\
	lynx.sgml	\
	nes.sgml	\
	pet.sgml	\
	plus4.sgml	\
        supervision.sgml\
	vic20.sgml

TXT  =	$(SGML:.sgml=.txt)
HTML =	$(SGML:.sgml=.html)
INFO =	$(SGML:.sgml=.info)
DVI  =	$(SGML:.sgml=.dvi)
TEX  =	$(SGML:.sgml=.tex)

# ------------------------------------------------------------------------------
# Pattern-rules, to make targets

%.txt:	%.sgml
	linuxdoc -B txt -f $(TXT_OPTIONS) $<

%.html:	%.sgml
	linuxdoc -B info --split=1 $(BUTTONS) $<

%.info:	%.sgml
	linuxdoc -B info $<

%.dvi:	%.sgml
	linuxdoc -B latex --output=dvi $<

%.tex:	%.sgml
	linuxdoc -B latex --output=tex $<

# ------------------------------------------------------------------------------
# Targets

.PHONY:	all
all:	txt html info dvi

.PHONY:	txt
txt:	linuxdoc $(TXT)

.PHONY:	html
html:	linuxdoc $(HTML)

.PHONY:	info
info:	linuxdoc $(INFO)

.PHONY:	dvi
dvi:	linuxdoc $(DVI)

.PHONY:	tex
tex:	linuxdoc $(TEX)

.PHONY:	linuxdoc
linuxdoc:
	@sgmlcheck index >/dev/null 2>&1 || { \
	echo; \
	echo '"LinuxDoc Tools" does not exist on this system.'; \
	echo 'So, most of the documentation might not have been built.'; \
	echo; \
	false;}

.PHONY:	clean
clean:
	$(RM) *~

.PHONY:	zap
zap:	clean
	$(RM) $(TXT) $(TEX) $(DVI) *.htm* *.inf* *.man

# ------------------------------------------------------------------------------
# Special target rules

coding.html:	coding.sgml
	sgml2html --split=0 $<

# funcref.sgml's third section is huge.
# So, funcref.html is split into sub-section files.
#
funcref.html:	funcref.sgml
	sgml2html --split=2 $(BUTTONS) $<

# The index.html target is special:
# It is only a table of contents.  So, it should not be split.
#
index.html:	index.sgml
	sgml2html --split=0 $<
