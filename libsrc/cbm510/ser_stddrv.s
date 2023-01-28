;
; Name of the standard serial driver
;
; Oliver Schmidt, 2022-12-22
;
; const char ser_stddrv[];
;

        .export _ser_stddrv

.rodata

_ser_stddrv:    .asciiz "cbm510-std.ser"
