# Program declarations
CC := cc65.exe
CA := ca65.exe
LD := ld65.exe
X64 := c:\winvice\x64.exe -autostart
X128 := c:\winvice\x128.exe -autostart
T := c64

# Default target - Builds for the C64
all: T := c64
all: hello.prg

# Build for the C128
c128: T := c128
c128: hello.prg

# Launch VICE x64 with our program.
vice64: T := c64
vice64: hello.prg
		$(X64) $(T)\$<

# Launch VICE x128 with our program
vice128: T := c128
vice128: hello.prg
		$(X128) $(T)\$<
		
# Build our program
hello.prg: Hello.o Text.o
		$(LD) -t $(T) -o $(T)\$@ $^ $(T).lib

# Assemble Hello
Hello.o: Hello.s
		$(CA) $^

# Compile Hello
Hello.s: Hello.c
		$(CC) -Oi -t $(T) $^

# Assemble our resources
Text.o: Text.s
		$(CA) -t $(T) $^

# Our phony targets for managing the project
.PHONY: clean clean128 cleanvs rebuild
clean: 
		rm -f *.o
		rm -f Hello.s
		rm -f $(T)/*.prg

clean128: T := c128
clean128: 
		rm -f *.o
		rm -f Hello.s
		rm -f $(T)/*.prg

# Visual Studio runs CMD.EXE, not Cygwin, so we need to use
# del instead of rm
cleanvs:
		del *.o
		del Hello.s
		del $(T)\*.prg

# Visual Studio has a nifty Rebuild feature 
rebuild: cleanvs all
