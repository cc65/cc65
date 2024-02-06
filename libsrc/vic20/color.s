;
; Ullrich von Bassewitz, 06.08.1998
; Adapted for Vic20 by Steve Schmidtke 05.08.2002
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export         _textcolor, _bgcolor, _bordercolor
        .importzp       tmp1

        .include        "vic20.inc"



.code

_textcolor:
        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts


_bgcolor:
        asl
        asl
        asl
        asl
        sta     tmp1
        sei                     ; don't want anything messing around while we update
        lda     VIC_COLOR       ; get old value
        and     #$0F
        tax
        ora     tmp1
        sta     VIC_COLOR       ; set new value
        cli
        txa
        lsr
        lsr
        lsr
        lsr
        rts


_bordercolor:
        and     #$07
        sta     tmp1
        sei                     ; don't want anything messing around while we update
        lda     VIC_COLOR       ; get old value
        and     #$F8
        tax
        ora     tmp1
        sta     VIC_COLOR       ; set new value
        cli
        txa
        rts

