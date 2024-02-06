;
; Ullrich von Bassewitz, 2005-03-28
;
; unsigned char __fastcall__ revers (unsigned char onoff)
;

        .export         _revers

        .include        "apple2.inc"

_revers:
        tax                     ; Test onoff
        beq     normal          ; If zero, "normal" must be set
        ldx     #$3F+1          ; Set "inverse"
normal: dex                     ; $00->$FF, $40->$3F
        lda     #$00            ; Preload return code for "normal"
        ldy     INVFLG          ; Load current flag value
        stx     INVFLG          ; Save new flag value
        bmi     :+              ; Jump if current value is $FF (normal)
        lda     #$01            ; Return "inverse"
:       ldx     #$00
        rts
