;
; Stefan 'MonteCarlos' Andree, 26.08.2024
;
; int __fastcall__ _set_working_screen(unsigned char screen_hi);
;


        .include        "c64.inc"

        .export         _cbm_set_working_screen

_cbm_set_working_screen:
        tax
        beq @error  ; Reject setting screen at zero page location
        and #3
        bne @error
        txa

        ldy #reject_range_count - 1
    @check_ranges:
            cmp reject_range_start, y
            bcc @accept
            cmp reject_range_end, y
            bcc @error
    @accept:
        dey
        bpl @check_ranges
    
        sta SCREEN_HI  ; Tell kernal to which memory screen output will go
        jsr $e56c

        lda #0
        tax
        rts
    @error:
        lda #$01
        ldx #$00
        rts

; Screen addr must not be set to $1000-$1fff nor $9000-$9fff for shadowing reasons and not to $d000-$dfff, due to registers
reject_range_start:
    .byte $d0, $90, $10
reject_range_end:
    .byte $e0, $a0, $20

reject_range_count = reject_range_end - reject_range_start
