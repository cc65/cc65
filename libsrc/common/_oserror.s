;
; Ullrich von Bassewitz, 16.05.2000
;
; extern unsigned char __oserror;
; /* Operating system specific errors from the low level functions */


        .export         ___oserror

.bss

___oserror:
        .res    1

