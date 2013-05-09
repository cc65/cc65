;
; Kevin Ruland
;
; char cgetc (void);
;
; If open_apple key is pressed then the high-bit of the key is set.
;

        .export _cgetc

        .include "apple2.inc"

_cgetc:
        lda     KBD
        bpl     _cgetc          ; If < 128, no key pressed

        ; At this time, the high bit of the key pressed is set
        bit     KBDSTRB         ; Clear keyboard strobe
        .ifdef __APPLE2ENH__
        bit     BUTN0           ; Check if OpenApple is down
        bmi     done
        .endif
        and     #$7F            ; If not down, then clear high bit
done:   ldx     #$00
        rts
