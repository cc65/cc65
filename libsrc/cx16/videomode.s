;
; 2019-11-06, Greg King
;
; /* Video mode defines */
; #define VIDEOMODE_40x30         0x00
; #define VIDEOMODE_80x60         0x02
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

        .import         SCRMOD


.proc   _videomode
        tax
        clc                     ; (Get old mode)
        jsr     SCRMOD
        pha
        txa

        sec                     ; (Set new mode)
        jsr     SCRMOD

        pla                     ; Get back old mode
        bcs     @L1
        ldx     #>$0000         ; Clear high byte
        rts

; The new mode is invalid.  Go back to the old mode.  Return -1.

@L1:    sec
        jsr     SCRMOD
        lda     #<-1
        tax
        rts
.endproc
