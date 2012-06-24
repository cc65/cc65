;
; Determine the CBM file type. From cbm_dir.c by Josef Soucek. Moved into an
; assembler function by Ullrich von Bassewitz 2012-06-03
;
; unsigned char __fastcall__ _cbm_filetype (unsigned char c);
;

        .include "cbm_filetype.inc"

        .macpack generic


; --------------------------------------------------------------------------
; Table with types for a list of start characters

.rodata
.proc   TypeTable
        .byte   CBM_T_CBM       ; c
        .byte   CBM_T_DEL       ; d
        .byte   CBM_T_OTHER     ; e
        .byte   CBM_T_OTHER     ; f
        .byte   CBM_T_OTHER     ; g
        .byte   CBM_T_OTHER     ; h
        .byte   CBM_T_OTHER     ; i
        .byte   CBM_T_OTHER     ; j
        .byte   CBM_T_OTHER     ; k
        .byte   CBM_T_LNK       ; l
        .byte   CBM_T_OTHER     ; m
        .byte   CBM_T_OTHER     ; n
        .byte   CBM_T_OTHER     ; o
        .byte   CBM_T_PRG       ; p
        .byte   CBM_T_OTHER     ; q
        .byte   CBM_T_REL       ; r
        .byte   CBM_T_SEQ       ; s
        .byte   CBM_T_OTHER     ; t
        .byte   CBM_T_USR       ; u
        .byte   CBM_T_VRP       ; v
.endproc


; --------------------------------------------------------------------------
; Mapper function

.code
.proc   __cbm_filetype

        ldx     #0              ; Clear high byte

; Check that the given char is in table range

        sec
        sbc     #'c'
        bcc     L1
        cmp     #.sizeof (TypeTable)
        bge     L1

; Ok, load the type

        tay
        lda     TypeTable,y
        rts

; Out of table range, return CBM_T_OTHER

L1:     lda     #CBM_T_OTHER
        rts

.endproc



