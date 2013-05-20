# Program declarations
CC := cc65.exe
CA := ca65.exe
LD := ld65.exe
C1541 := c:\winvice\c1541.exe
X64 := c:\winvice\x64.exe -autostart
X128 := c:\winvice\x128.exe -autostart
T := c128

SOURCES	= \
	Hello.c

ASMFILES= \
	res\$(T)\Text.s \
	res\common\Common.s

PROGRAM	=hello.prg

TITLE	=$(PROGRAM:.prg=)

LIBS	=$(T).lib

# Default target - Builds for the C64
all: $(PROGRAM)

# Launch VICE
vice: clean $(PROGRAM)
ifeq "$(T)" "c64"
		$(X64) $(T)\$(PROGRAM)
else
		$(X128) $(T)\$(PROGRAM)
endif

D64: all
	cp $(T)\$(PROGRAM) $(PROGRAM)
	$(C1541) -format $(TITLE),id d64 $(TITLE).d64
	$(C1541) $(TITLE).d64 -write $(PROGRAM) $(PROGRAM)
	rm $(PROGRAM)

# Build our program
 $(PROGRAM): $(SOURCES:.c=.o) $(ASMFILES:.s=.o)
		$(LD) -t $(T) -o $(T)\$@ $^ $(LIBS)

# Assemble 
$(SOURCES:.c=.o): $(SOURCES:.c=.s)
	$(foreach F, $(SOURCES:.c=.s), \
		$(shell	$(CA) -t $(T) $(F)))

# Compile 
$(SOURCES:.c=.s): $(SOURCES)
		$(CC) -Oi -t $(T) $^

# Assemble our resources
$(ASMFILES:.s=.o): $(ASMFILES)
	$(foreach F, $(ASMFILES), \
		$(shell	$(CA) -t $(T) $(F)))

# Our phony targets for managing the project
#.PHONY: clean
clean: 
		$(RM) $(SOURCES:.c=.o) $(SOURCES:.c=.s)
		$(RM) $(ASMFILES:.s=.o)
		$(RM) $(T)/$(PROGRAM)

