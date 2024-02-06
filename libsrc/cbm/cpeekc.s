;
; 2016-02-28, Groepaz
; 2017-06-22, Greg King
;
; char cpeekc (void);
;

        .export         _cpeekc

; Get a system-specific file.
; Note:  The cbm610, and c128 targets need special
; versions that handle RAM banking and the 80-column VDC.

.if     .def(__C16__)
        .include        "plus4.inc"     ; both C16 and Plus4
.elseif .def(__C64__)
        .include        "c64.inc"
.elseif .def(__CBM510__)
        .import         CURS_X: zp, SCREEN_PTR: zp
        .include        "cbm510.inc"
.elseif .def(__PET__)
        .include        "pet.inc"
.elseif .def(__VIC20__)
        .include        "vic20.inc"
.endif


_cpeekc:
        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; get screen code
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
