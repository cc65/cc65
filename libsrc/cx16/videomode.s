;
; 2022-03-28, Greg King
;
; /* Video mode defines */
; #define VIDEOMODE_80x60         0x00
; #define VIDEOMODE_80x30         0x01
; #define VIDEOMODE_40x60         0x02
; #define VIDEOMODE_40x30         0x03
; #define VIDEOMODE_40x15         0x04
; #define VIDEOMODE_20x30         0x05
; #define VIDEOMODE_20x15         0x06
; #define VIDEOMODE_320x240       0x80
; #define VIDEOMODE_SWAP          (-1)
;
; signed char __fastcall__ videomode (signed char Mode);
; /* Set the video mode, return the old mode.
; ** Return -1 if Mode isn't valid.
; ** Call with one of the VIDEOMODE_xx constants.
; */
;

        .export         _videomode

        .import         SCREEN_MODE


.proc   _videomode
        sec                     ; Get old mode
        pha
        jsr     SCREEN_MODE
        plx
        pha                     ; Save old mode
        txa
        clc                     ; Set new mode
        jsr     SCREEN_MODE

        pla                     ; Get back old mode
        ldx     #>$0000         ; Clear high byte
        bcc     @L1

; The new mode is invalid.  Return -1.

        dex
        txa
@L1:    rts
.endproc
