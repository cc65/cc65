;
; Oliver Schmidt, 2013-05-16
;
; extern int errno;
;

        .include        "errno.inc"

        .bss

__errno:
        .word   0
