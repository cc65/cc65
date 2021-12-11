;
; Ullrich von Bassewitz, 2003-08-12, 2005-04-16
;
; Place to store the current working directory.
;
; __cwd is initialized by a platform specific function named "initcwd" called
; by the constructor defined in this module.
;

        .export         __cwd
        .export         __cwd_buf_size
        .constructor    cwd_init

        .import         initcwd

        .include        "stdio.inc"

        __cwd_buf_size  = FILENAME_MAX

        cwd_init        := initcwd

.segment        "INIT"

__cwd:  .res    __cwd_buf_size


; NOTE: Some of the code working with directories is not able to handle
; strings longer than 255 chars, so don't make __cwd larger than 256 without
; checking the other sources.

        .assert __cwd_buf_size < 256, error, "__cwd_buf_size must not be > 255"
