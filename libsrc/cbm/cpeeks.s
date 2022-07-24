;
; 2017-07-05, Greg King
;
; void cpeeks (char* s, unsigned length);
;

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

        .macpack        generic

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


_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr3            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr3+1

        lda     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sta     ptr2
        stx     ptr2+1
        ldy     CURS_X
        sty     tmp2

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1
        ldx     #<$0000
        stx     ptr1
        bze     L3              ; branch always

L4:     ldy     tmp2
        lda     (ptr2),y        ; get char
        iny
        bnz     L2
        inc     ptr2+1
L2:     sty     tmp2
        and     #<~$80          ; remove reverse bit

; Convert the screen code into a PetSCII code.
; $00 - $1F: +$40
; $20 - $3F
; $40 - $5f: +$20
; $60 - $7F: +$40

        cmp     #$20
        blt     @sk1            ;(bcc)
        cmp     #$40
        blt     L5
        cmp     #$60
        blt     @sk2            ;(bcc)
        clc
@sk1:   adc     #$20
@sk2:   ;clc                    ; both above cmp and adc clear carry flag
        adc     #$20

L5:     ldy     tmp1
        sta     (ptr1),y
        iny
        bnz     L1
        inc     ptr1+1
L1:     sty     tmp1

L3:     inc     ptr3            ; count length
        bnz     L4
        inc     ptr3+1
        bnz     L4

        txa                     ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts
