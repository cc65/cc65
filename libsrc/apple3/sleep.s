;
; Robert Justice, 2026
;
; void __fastcall__ sleep(unsigned s)
;
;

        .export         _sleep
        .import         _waitvsync
        .importzp       tmp1


_sleep:
        stx     tmp1            ; High byte of s in X
        tay                     ; Low byte in A
        ora     tmp1
        bne     :+
        rts
:

sleep_1s:
        ldx     #60             ; Loop 60 times
:       jsr     _waitvsync
        dex
        bne     :-
        dey
        bne     sleep_1s
        dec     tmp1
        bmi     done
        dey                     ; Down to #$FF
        bne     sleep_1s

done:   rts