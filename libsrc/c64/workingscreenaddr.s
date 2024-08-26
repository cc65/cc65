;
; Stefan 'MonteCarlos' Andree, 26.08.2024
;
; int __fastcall__ _set_working_screen(unsigned char screen_hi);
;


        .include        "c64.inc"

        .import         PLOT
        .export         _cbm_set_working_screen, _cbm_reset_working_screen

_cbm_set_working_screen:
        ldx screen_selected  ; If selected screen is already alternate screen, then error
        bne @error

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

        ldx SCREEN_HI
        stx old_screen_hi
        sta SCREEN_HI  ; Tell kernal to which memory screen output will go
        sec
        jsr PLOT
        stx old_crsr_row
        sty old_crsr_col
        ldx alt_crsr_row
        ldy alt_crsr_col
        clc
        jsr PLOT

        inc screen_selected

        lda #0
        tax
        rts
    @error:
        lda #$01
        ldx #$00
        rts


_cbm_reset_working_screen:
        lda screen_selected  ; If original screen is already alternate screen, then error
        beq @error
        lda old_screen_hi
        sta SCREEN_HI

        sec
        jsr PLOT
        stx alt_crsr_row
        sty alt_crsr_col

        ldx old_crsr_row
        ldy old_crsr_col
        clc
        jsr PLOT

        dec screen_selected
        
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

screen_selected:
    .byte 0
old_crsr_row:
    .res 1
old_crsr_col:
    .res 1
old_screen_hi:
    .res 1
alt_crsr_row:
    .byte 0
alt_crsr_col:
    .byte 0
