;
; Ullrich von Bassewitz, 2003-08-12
;
; Place to store the current working directory.
;

       	.export		__cwd
        .export         __cwd_buf_size

        __cwd_buf_size  = 64

.bss

__cwd:  .res	__cwd_buf_size


