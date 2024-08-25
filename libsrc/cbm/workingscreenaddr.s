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
