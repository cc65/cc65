;
; 2020-01-06, Greg King
;
; /* Video mode defines */
; #define VIDEOMODE_40x30         0x00
; #define VIDEOMODE_80x60         0x02
; #define VIDEOMODE_320x200       0x80
; #define VIDEOMODE_SWAP          (-1)
;
; signed char __fastcall__ videomode (signed char Mode);
; /* Set the video mode, return the old mode.
; ** Return -1 if Mode isn't valid.
; ** Call with one of the VIDEOMODE_xx constants.
; */
;

        .export         _videomode

        .import         SCREEN_SET_MODE
        .include        "cx16.inc"


.proc   _videomode
        ldx     SCREEN_MODE     ; Get old mode
        phx

        jsr     SCREEN_SET_MODE

        pla                     ; Get back old mode
        ldx     #>$0000         ; Clear high byte
        bcs     @L1
        rts

; The new mode is invalid.  Go back to the old one.  Return -1.

@L1:    sta     SCREEN_MODE
        dex
        txa
        rts
.endproc
