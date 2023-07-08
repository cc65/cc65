;
; Name of the standard serial driver
;
; Oliver Schmidt, 2022-12-22
;
; const char ser_stddrv[];
;

        .export _ser_stddrv

.rodata

_ser_stddrv:
        .ifdef  __ATARIXL__
                .asciiz "atrxrdev.ser"
        .else
                .asciiz "atrrdev.ser"
        .endif
