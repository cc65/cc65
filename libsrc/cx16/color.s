;
; 2019-09-16, Greg King
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
;


        .export         _textcolor, _bgcolor

        .importzp       tmp1
        .include        "cx16.inc"

_textcolor:
        and     #$0F
        sta     tmp1
        ldx     CHARCOLOR       ; get old values
        txa
        and     #<~$0F          ; keep screen color, remove text color
        ora     tmp1
        sta     CHARCOLOR       ; set new values
        txa
        and     #$0F
        rts


_bgcolor:
        asl     a               ; move number to screen-color nybble
        asl     a
        asl     a
        asl     a
        sta     tmp1
        ldx     CHARCOLOR       ; get old values
        txa
        and     #<~$F0          ; remove screen color, keep text color
        ora     tmp1
        sta     CHARCOLOR       ; set new values
        txa
        lsr     a               ; get old background color
        lsr     a
        lsr     a
        lsr     a
        rts
