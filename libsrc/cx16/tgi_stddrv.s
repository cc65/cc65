;
; Name of the standard TGI driver
;
; 2020-06-04, Greg King
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

; A FAT32 8+3 file-name (for SD cards)

.rodata
_tgi_stddrv:    .asciiz "cx320p1.tgi"
