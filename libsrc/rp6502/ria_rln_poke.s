;
; int __fastcall__ ria_rln_poke (const char* poke);
;

        .export         _ria_rln_poke

        .import         _strlen

        .importzp       ptr1

        .include        "rp6502.inc"
        .include        "errno.inc"

_ria_rln_poke:
        sta     ptr1
        stx     ptr1+1
        jsr     _strlen         ; X:Y = length
        cpy     #1
        txa
        sbc     #>$200
        bcs     inval           ; length > $200
        txa
        adc     ptr1+1
        sta     ptr1+1          ; poke + (length & $FF00)
        tya
        beq     @page
@push:  dey
        lda     (ptr1),y
        sta     RIA_XSTACK
        tya
        bne     @push
@page:  dec     ptr1+1
        dex
        bpl     @push
        lda     #RIA_OP_RLN_POKE
        sta     RIA_OP
        jmp     RIA_SPIN
inval:  lda     #EINVAL
        jmp     ___directerrno
