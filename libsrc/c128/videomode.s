;
; Ullrich von Bassewitz, 2009-09-07
;
; unsigned __fastcall__ videomode (unsigned Mode);
; /* Set the video mode, return the old mode */
;

        .export         _videomode
        .import         SWAPPER, BSOUT

        .include        "c128.inc"


.proc   _videomode

        cmp     MODE                    ; Do we have this mode already?
        beq     @L9

        lda     MODE                    ; Get current mode ...
        pha                             ; ... and save it

        jsr     SWAPPER                 ; Toggle the mode
        lda     #14
        jsr     BSOUT                   ; Switch to lower case chars

        pla                             ; Get old mode into A

; Done, old mode is in A

@L9:    ldx     #$00                    ; Clear high byte
        rts

.endproc

