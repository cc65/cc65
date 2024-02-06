;
; 2016-02-28, Groepaz
; 2017-06-22, Greg King
;
; unsigned char cpeekcolor (void);
;

        .export         _cpeekcolor

; Get a system-specific file.
; Note:  The cbm510, cbm610, c128, and Pet targets need special
; versions that handle RAM banking, the 80-column VDC, and monochrome.

.if     .def(__C16__)
        .include        "plus4.inc"     ; both C16 and Plus4
.elseif .def(__C64__)
        .include        "c64.inc"
.elseif .def(__VIC20__)
        .include        "vic20.inc"
.endif


_cpeekcolor:
        ldy     CURS_X
        lda     (CRAM_PTR),y    ; get color
        and     #$0F
        ldx     #>$0000
        rts
