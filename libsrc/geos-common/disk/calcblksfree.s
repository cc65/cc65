;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; int CalcBlksFree (void);

            .import ___oserror
            .export _CalcBlksFree

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"

_CalcBlksFree:
        lda #<curDirHead
        ldx #>curDirHead
        sta r5L
        stx r5H
        jsr CalcBlksFree
        stx ___oserror
        lda r4L
        ldx r4H
        rts
