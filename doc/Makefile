#
# Makefile for the cc65 documentation
#


# Default for the compiler lib search path as compiler define

SGML  =	apple2.sgml	\
	ar65.sgml 	\
        atari.sgml      \
        c128.sgml       \
        c16.sgml        \
        c64.sgml        \
	ca65.sgml 	\
	ca65html.sgml	\
        cbm610.sgml     \
	cc65.sgml 	\
	cl65.sgml   	\
        co65.sgml       \
	coding.sgml	\
        da65.sgml       \
	debugging.sgml	\
	dio.sgml    	\
        funcref.sgml    \
	geos.sgml   	\
	index.sgml  	\
	intro.sgml	\
	ld65.sgml   	\
	library.sgml    \
        lynx.sgml       \
        plus4.sgml      \
        vic20.sgml

TXT   =	$(SGML:.sgml=.txt)
HTML  = $(SGML:.sgml=.html)
INFO  = $(SGML:.sgml=.info)
DVI   = $(SGML:.sgml=.dvi)
TEX   = $(SGML:.sgml=.tex)

# ------------------------------------------------------------------------------
# Rules to make targets

%.txt:	%.sgml
	sgml2txt $<

%.html:	%.sgml
	sgml2html --split=1 $<

%.info:	%.sgml
	sgml2info $<

%.dvi:	%.sgml
	sgml2latex $<

%.tex:	%.sgml
      	sgml2latex --output=tex $<

# ------------------------------------------------------------------------------
# Targets

.PHONY: all
all:  	txt html info dvi

.PHONY:	txt
txt:  	$(TXT)

.PHONY:	html
html: 	$(HTML)

.PHONY:	info
info: 	$(INFO)

.PHONY:	dvi
dvi:  	$(DVI)

.PHONY:	tex
tex:  	$(TEX)

# The index.html target is special, since it does not get splitted
index.html:	index.sgml
      	sgml2html --split=0 $<

clean:
	rm -f *~

zap:	clean
	rm -f $(TXT) $(HTML) $(INFO) $(DVI) $(TEX) *.html *.info-* *.man

# ------------------------------------------------------------------------------
# Special target rules

coding.html:	coding.sgml
	sgml2html --split=0 $<

funcref.html:	funcref.sgml
	sgml2html --split=2 $<

index.html:    	index.sgml
	sgml2html --split=0 $<

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -MM $^ > .depend


