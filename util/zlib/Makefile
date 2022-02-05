
.PHONY: mostlyclean clean

zlib: warning
#zlib: deflater

warning:
	@echo "deflater needs zlib installed, use 'make deflater' to build"

deflater: deflater.c
	$(CC) $(CFLAGS) -o deflater deflater.c -lz

mostlyclean clean:
	$(RM) deflater
	
install zip:

