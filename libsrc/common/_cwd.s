;
; Ullrich von Bassewitz, 2003-08-12
;
; Place to store the current working directory.
; NOTE: Some of the code working with directories is not able to handle 
; strings longer than 255 chars, so don't make __cwd_buf_size larger than 256
; without checking the other sources.

       	.export		__cwd
        .export         __cwd_buf_size

        __cwd_buf_size  = 64

.bss

__cwd:  .res	__cwd_buf_size


