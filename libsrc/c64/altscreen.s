;
; Stefan 'MonteCarlos' Andree, 26.08.2024
;
; int __fastcall__ _init_alt_screen(unsigned char screen_hi, char fillchar)
; int __fastcall__ _set_alt_screen(void);
;


        .include        "c64.inc"

        .importzp       sp, ptr1
        .import         popa, PLOT
        .import         _restorecursorfrom, _savecursorto
        .export         _cbm_init_alt_screen, _cbm_set_alt_screen, _cbm_set_def_screen


.proc _cbm_set_alt_screen
        ldx     screen_selected     ; If alternate screen is already set, then error
        sec                         ; set error flag
        bne     @return

        ldx     SCREEN_HI
        stx     old_screen_hi

        lda     alt_screen_addr
        beq     @return             ; not initialized
        sta     SCREEN_HI           ; Tell kernal to which memory screen output will go

        lda     #<def_screen_crsr_pos
        ldx     #>def_screen_crsr_pos
        jsr     _savecursorto

        lda     #<alt_screen_crsr_pos
        ldx     #>alt_screen_crsr_pos
        jsr     _restorecursorfrom

        inc     screen_selected

        clc                         ; clear error flag
    @return:
        jmp     return_exit_status
.endproc


.proc _cbm_set_def_screen
        lda     screen_selected     ; If original screen is already set, then error
        sec                         ; set error flag
        beq     return_exit_status

        lda     old_screen_hi
        sta     SCREEN_HI

        lda     #<def_screen_crsr_pos
        ldx     #>def_screen_crsr_pos
        jsr     _restorecursorfrom

        dec     screen_selected

        clc                         ; clear error fla
        bcc     return_exit_status
.endproc


.proc _cbm_init_alt_screen
        jsr     check_screen_addr
        bcs     @return

        ; Pay attention, that carry is clear from here on

        sta     alt_screen_addr
        sta     ptr1 + 1
        ldy     #0
        sty     ptr1

        lda     (sp), y             ; fetch fillchar (does not influence carry)
        ldy     #0
        ldx     #4                  ; This may depend on the target system
    @fill_screen:
        sta     (ptr1), y
        iny
        bne     @fill_screen

        inc     ptr1 + 1
        dex
        bne     @fill_screen

        stx     alt_screen_crsr_pos ; Init cursor pos to 0/0 on alt screen
        stx     alt_screen_crsr_pos + 1

    @return:
        jsr     popa
        jmp     return_exit_status
.endproc


.proc check_screen_addr
        tax
        beq     @error              ; Reject setting screen at zero page location
        and     #3
        bne     @error
        txa

        ldx #reject_range_count - 1
    @check_ranges:
            cmp reject_range_start, x
            bcc @accept
            cmp reject_range_end, x
            bcc @error
    @accept:
        dex
        bpl @check_ranges

        clc
        rts

    @error:
        sec
        rts
.endproc


.proc return_exit_status
        ; return EXIT_SUCCESS, if clc
        ; return EXIT_FAILURE, if sec
        lda     #0
        tax
        bcc     :+
        adc     #0
    :   rts
.endproc


; Screen addr must not be set to $1000-$1fff nor $9000-$9fff for shadowing reasons and not to $d000-$dfff, due to registers
reject_range_start:
    .byte   $d0, $90, $10
reject_range_end:
    .byte   $e0, $a0, $20
reject_range_count = reject_range_end - reject_range_start


alt_screen_addr:
    .byte   0
screen_selected:
    .res    1
def_screen_crsr_pos:
    .res    2
old_screen_hi:
    .res    1
alt_screen_crsr_pos:
    .res    2