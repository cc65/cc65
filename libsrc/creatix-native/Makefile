# Makefile for creatix-native.lib

TARGET = creatix-native

AS = ../../bin/ca65
ASFLAGS = -t creatix-native -I ../../asminc

AR65 = ../../bin/ar65

WRKDIR = ../../libwrk/$(TARGET)
LIBDIR = ../../lib

LIB = $(LIBDIR)/$(TARGET).lib

OBJS = $(WRKDIR)/_read.o $(WRKDIR)/_write.o $(WRKDIR)/copydata.o $(WRKDIR)/zerobss.o $(WRKDIR)/zeropage.o

all: $(LIB)

clean:
	rm -f $(WRKDIR)/*
	rm -f $(LIB)

$(WRKDIR)/%.o: %.s | $(WRKDIR)
	test -d $(WRKDIR) || mkdir $(WRKDIR)
	$(AS) -g -o $@ $(ASFLAGS) $<	

$(LIB): $(OBJS)
	cd $(WRKDIR)
	$(AR65) a $@ $?

.SUFFIXES: .s .o 
