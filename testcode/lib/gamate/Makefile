
all: audiotest.bin lcdtest.bin ctest.bin

audiotest.bin: audiotest.s
	../../../bin/cl65 -l audiotest.lst -t gamate -o audiotest.bin audiotest.s
lcdtest.bin: lcdtest.s
	../../../bin/cl65 -l lcdtest.lst -t gamate -o lcdtest.bin lcdtest.s
ctest.bin: ctest.c
	../../../bin/cl65 -l ctest.lst -t gamate -o ctest.bin ctest.c

test1: lcdtest.bin
	cd ~/Desktop/mame/winmess/ && wine mess.exe gamate -window -skip_gameinfo -cart ~/Desktop/cc65/github/cc65/testcode/lib/gamate/lcdtest.bin
test2: audiotest.bin
	cd ~/Desktop/mame/winmess/ && wine mess.exe gamate -window -skip_gameinfo -cart ~/Desktop/cc65/github/cc65/testcode/lib/gamate/audiotest.bin
testc: ctest.bin
	cd ~/Desktop/mame/winmess/ && wine mess.exe gamate -window -skip_gameinfo -cart ~/Desktop/cc65/github/cc65/testcode/lib/gamate/ctest.bin

clean:
	rm -f lcdtest.o audiotest.o ctest.o
	rm -f lcdtest.bin audiotest.bin ctest.bin
