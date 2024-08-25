;
; Stefan 'MonteCarlos' Andree, 26.08.2024
;
; int __fastcall__ _set_working_screen(unsigned char screen_hi);
;

        .include        "cbm.inc"
        .include        "c64.inc"

        .export         _cbm_set_working_screen

_cbm_set_working_screen:
        tax
        and #3
        bne @error
        cpx #$10    ; Screen addr must not be set to $1000-$1fff nor $9000-$9fff
        bcc @ok_1000
        cpx #$1f
        bcc @error
    @ok_1000:
        cpx #$90    ; Screen addr must not be set to $1000-$1fff nor $9000-$9fff
        bcc @ok_9000
        cpx #$9f
        bcc @error
    @ok_9000:
        stx SCREEN_HI
        sec
        jsr PLOT    ; Get cursor position ...
        clc
        jsr PLOT    ; ... and set it again to update address of char below cursor with new hi-byte

        lda #0
        tax
        rts
    @error:
        lda #$ff
        tax
        rts
