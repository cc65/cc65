
all: audiotest.bin lcdtest.bin

audiotest.bin: audiotest.s
#	../../../bin/ca65 -t gamate -o audiotest.o audiotest.s
#	../../../bin/ld65 -t gamate -o audiotest.bin audiotest.o
	../../../bin/cl65 -t gamate -o audiotest.bin audiotest.s

lcdtest.bin: lcdtest.s
#	../../../bin/ca65 -t gamate -o lcdtest.o lcdtest.s
#	../../../bin/ld65 -t gamate -o lcdtest.bin lcdtest.o
	../../../bin/cl65 -l bla.lst -t gamate -o lcdtest.bin lcdtest.s

test1: lcdtest.bin
	cd /home/groepaz/Desktop/mame/winmess/ && wine mess.exe gamate -window -skip_gameinfo -cart /home/groepaz/Desktop/cc65/github/cc65/testcode/lib/gamate/lcdtest.bin
test2: audiotest.bin
	cd /home/groepaz/Desktop/mame/winmess/ && wine mess.exe gamate -window -skip_gameinfo -cart /home/groepaz/Desktop/cc65/github/cc65/testcode/lib/gamate/audiotest.bin

clean:
	rm -f lcdtest.o audiotest.o
	rm -f lcdtest.bin audiotest.bin
