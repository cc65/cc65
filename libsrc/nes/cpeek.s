;
; 2016-02-28, Groepaz
; 2017-08-17, Greg King
; 2026-09-18, Brandon Woodward
; char cpeekc (void);
;
; 2016-02-28, Groepaz
; 2017-08-17, Greg King
; 2026-09-18, Brandon Woodward
; char cpeekrevers (void);
;
; 2017-06-03, Greg King
; unsigned char cpeekcolor (void);
;

        .import         return1
        .export         _cpeekcolor := return1  ; always COLOR_WHITE

        .forceimport    initconio
        .import         _waitvsync
        .export         _cpeekc, _cpeekrevers

        .include        "nes.inc"
        .forceimport    initconio

_cpeekrevers:

        ldy     #$80
        bne     L1

_cpeekc:
        ldy     #$7F          ; remove reverse bit

L1:     lda     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sta     ppuld_lo
        stx     ppuld_hi

; waiting for vblank is incredibly slow... but it needs to be done
        jsr     _waitvsync

        tya
        and     ppuld_val
        rts
