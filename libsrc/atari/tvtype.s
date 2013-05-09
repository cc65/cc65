;
; Christian Groessler, July 2004
;
; unsigned char get_tv(void)
;
; returns the TV system the machine is using
; 0 - NTSC
; 1 - PAL
;


        .include        "atari.inc"
        .include        "get_tv.inc"

.proc   _get_tv

        ldx     #TV::NTSC       ; Assume NTSC
        lda     PAL             ; use hw register, PALNTS is only supported on XL/XE ROM
        and     #$0e
        bne     @NTSC
        inx                     ; = TV::PAL
@NTSC:  txa
        ldx     #0              ; Expand to int
        rts

.endproc
