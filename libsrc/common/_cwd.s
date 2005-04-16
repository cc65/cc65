;
; Ullrich von Bassewitz, 2003-08-12, 2005-04-16
;
; Place to store the current working directory.
; NOTE: Some of the code working with directories is not able to handle
; strings longer than 255 chars, so don't make __cwd_buf_size larger than 256
; without checking the other sources.
;
; __cwd is initialized by a platform specific function named "initcwd" called
; by the constructor defined in this module.
;

       	.export		__cwd
        .export         __cwd_buf_size
        .constructor    cwd_init

        .import         initcwd

        __cwd_buf_size  = 64
        cwd_init        := initcwd

.bss

__cwd:  .res	__cwd_buf_size


