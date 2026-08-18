;
; Ullrich von Bassewitz, 2005-03-28
; Robert Justice, 2026
;
; unsigned char __fastcall__ revers (unsigned char onoff)
;

        .export         _revers
        .import         putcdirect, consinvflg

        .include        "apple3.inc"

_revers:
        tax                     ; Test onoff
        beq     normal          ; If zero, "normal" must be set
        lda     #CONSOLE_INVERSE
        bne     :+
normal: lda     #CONSOLE_NORMAL
:       jsr     putcdirect
        lda     #$00            ; Preload return code for "normal"
        ldy     consinvflg      ; Load current flag value
        stx     consinvflg      ; Save new flag value
        beq     :+              ; Jump if current value is normal
        lda     #$01            ; Return "inverse"
:       ldx     #>$0000
        rts
