; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick, BLL kit mikey.mac
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; Ullrich von Bassewitz, 2004-10-09, small changes.
;
; void __fastcall__ lynx_change_framerate (unsigned char rate);
; /* Change the framerate, in Hz.  Recognized values are 50, 60 and 75. */
;

        .include    "lynx.inc"
	.export     _lynx_change_framerate

	.code


_lynx_change_framerate:
       	cmp     #75
       	beq     set_75
       	cmp     #60
       	beq     set_60
        cmp     #50
       	bne     exit

set_50: lda     #$bd
       	ldx     #$31
       	bra     doit

set_60: lda     #$9e
       	ldx     #$29
       	bra     doit

set_75: lda     #$7e
       	ldx     #$20

doit:   sta     HTIMBKUP
       	stx     PBKUP

exit:   rts


