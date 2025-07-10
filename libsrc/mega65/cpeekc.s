;
; 2016-02-28, Groepaz
; 2017-06-22, Greg King
;
; char cpeekc (void);
;

        .include        "mega65.inc"

        .export         _cpeekc
        .importzp       ptr1

_cpeekc:
        lda     SCREEN_PTR + 1
        clc
        adc     #>$0800
        sta     ptr1 + 1
        lda     SCREEN_PTR
        sta     ptr1

        ldy     CURS_X
        lda     (ptr1),y  ; get screen code
        ldx     #>$0000
        and     #<~$80          ; remove reverse bit

; Convert the screen code into a PetSCII code.
; $00 - $1F: +$40
; $20 - $3F
; $40 - $5f: +$20
; $60 - $7F: +$40

        cmp     #$20
        bcs     @sk1            ;(bge)
        ora     #$40
        rts

@sk1:   cmp     #$40
        bcc     @end            ;(blt)
        cmp     #$60
        bcc     @sk2            ;(blt)
        ;sec
        adc     #$20 - $01
@sk2:   ;clc                    ; both above cmp and adc clear carry flag
        adc     #$20
@end:   rts
