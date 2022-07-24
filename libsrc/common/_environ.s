;
; Ullrich von Bassewitz, 2005-04-21
;
; extern char** _environ;
;
;
; __environ is a pointer to the environment.
; __envcount is the number of entries in the environment.
; __envsize is the size of the allocated array.
;
; The maximum number of environment entries is 64. putenv() will set errno
; to ENOMEM when trying to increase the number beyond this limit.
;

        .export         __environ, __envcount, __envsize
        .import         initenv
        .constructor    env_init

        env_init := initenv

.data

__environ:
        .addr   0
__envcount:
        .byte   0
__envsize:
        .byte   0
