;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99

; char DlgBoxFileSelect       (char *class, char ftype, char *fname);

            .export _DlgBoxFileSelect
            .import popa, popax
            .import _DoDlgBox

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
            .include "geosmac.inc"
            .include "const.inc"

_DlgBoxFileSelect:
        sta tmp_r5
        stx tmp_r5+1
        jsr popa
        sta tmp_r7L
        jsr popax
        sta tmp_r10
        stx tmp_r10+1

DB_FS_reload:
        MoveW tmp_r5, r5
        MoveW tmp_r10, r10
        MoveB tmp_r7L, r7L

        lda #<paramStrFileSelect
        ldx #>paramStrFileSelect
        jsr _DoDlgBox
        cmp #DISK
        bne DB_FS_Fin
        jsr OpenDisk
        txa
        beq DB_FS_reload
DB_FS_Fin:
        rts

.rodata

paramStrFileSelect:
        .byte DEF_DB_POS | 1
        .byte DBGETFILES, 4, 4
        .byte OPEN, DBI_X_2, DBI_Y_0+16
        .byte DISK, DBI_X_2, DBI_Y_0+32+1
        .byte CANCEL, DBI_X_2, DBI_Y_0+64+3
        .byte NULL

.bss

tmp_r5:
        .res 2
tmp_r7L:
        .res 1
tmp_r10:
        .res 2
