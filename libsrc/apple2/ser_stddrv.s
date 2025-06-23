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
        .ifdef  __APPLE2ENH__
        .asciiz "A2E.SSC.SER"
        .else
        .asciiz "A2.SSC.SER"
        .endif
