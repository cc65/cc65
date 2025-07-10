;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; void __fastcall__ sleep(unsigned s)
;
;

        .export         _sleep
        .import         _get_iigs_speed
        .import         _set_iigs_speed
        .import         WAIT
        .importzp       tmp1

        .include        "accelerator.inc"

        ; This functions uses the Apple2 WAIT ROM routine to waste a certain
        ; amount of cycles and returns approximately after the numbers of
        ; seconds passed in AX.
        ;
        ; It takes 1023730 cycles when called with AX=1 (1,0007s),
        ; 10236364 cycles when called with AX=10 (10,006 seconds),
        ; 306064298 cycles with AX=300 (299.2 seconds).
        ;
        ; Caveat: IRQs firing during calls to sleep will make the sleep longer
        ; by the amount of cycles it takes to handle the IRQ.
        ;
_sleep:
        stx     tmp1            ; High byte of s in X
        tay                     ; Low byte in A
        ora     tmp1
        bne     :+
        rts
:       jsr     _get_iigs_speed ; Save current CPU speed
        pha
        lda     #SPEED_SLOW     ; Down to 1MHz for consistency around WAIT
        jsr     _set_iigs_speed
sleep_1s:
        ldx     #$0A            ; Loop 10 times
sleep_100ms:
        lda     #$C7            ; Sleep about 99ms
        jsr     WAIT
        lda     #$0D             ; About 1ms
        jsr     WAIT
        dex
        bne     sleep_100ms
        dey
        bne     sleep_1s
        dec     tmp1
        bmi     done
        dey                     ; Down to #$FF
        bne     sleep_1s
done:
        pla                     ; Restore CPU speed
        jmp     _set_iigs_speed
