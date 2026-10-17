;
; struct tm* __fastcall__ localtime (const time_t* timep);
;

        .export         _localtime

        .import         __rp6502_tm_call
        .import         ldeaxi

        .importzp       sreg

        .include        "rp6502.inc"

_localtime:
        jsr     ldeaxi          ; A:X:sreg = *timep (32-bit load)
        stz     RIA_XSTACK      ; zero on top keeps the unsigned time_t positive
        ldy     sreg+1
        sty     RIA_XSTACK
        ldy     sreg
        sty     RIA_XSTACK
        stx     RIA_XSTACK
        sta     RIA_XSTACK
        lda     #RIA_OP_LOCALTIME
        jmp     __rp6502_tm_call
