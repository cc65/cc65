;
; 2016-02-28, Groepaz
; 2020-04-29, Greg King
;
; char cpeekc (void);
; /* Return the character from the current cursor position. */
;

        .export         _cpeekc

        .include        "cx16.inc"


_cpeekc:
        stz     VERA::CTRL      ; use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; set row number
        stz     VERA::ADDR+2
        lda     CURS_X          ; get character column
        asl     a               ; each character has two bytes
        sta     VERA::ADDR
        lda     VERA::DATA0     ; get screen code
        ldx     #>$0000
        and     #<~%10000000    ; remove reverse bit

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
