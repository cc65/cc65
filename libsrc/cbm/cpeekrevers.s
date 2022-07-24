;
; 2016-02-28, Groepaz
; 2017-06-15, Greg King
;
; unsigned char cpeekrevers (void);
;

        .export         _cpeekrevers

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


_cpeekrevers:
        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; get screen code
        and     #$80            ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
