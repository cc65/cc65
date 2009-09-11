;
; Oliver Schmidt, 10.9.2009
;
; Default ProDOS 8 I/O buffer management
; 

        .export		iobuf_alloc, iobuf_free, iobuf_mli
        .import		_posix_memalign, _free

iobuf_alloc := _posix_memalign
iobuf_free  := _free
iobuf_mli   := $BF00
