;
; Ullrich von Bassewitz, 16.05.2000
;
; extern unsigned char _oserror;
; /* Operating system specific errors from the low level functions */


        .export         __oserror

.bss

__oserror:
        .res    1

