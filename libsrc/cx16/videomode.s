;
; 2009-09-07, Ullrich von Bassewitz
; 2019-09-23, Greg King
;
; unsigned __fastcall__ videomode (unsigned Mode);
; /* Set the video mode, return the old mode. */
;

        .export         _videomode
        .import         SWAPPER

        .include        "cx16.inc"


.proc   _videomode
        cmp     LLEN                    ; Do we have this mode already?
        beq     @L9

        lda     LLEN                    ; Get current mode ...
        pha                             ; ... and save it

        jsr     SWAPPER                 ; Toggle the mode

        pla                             ; Get old mode into A

; Done, old mode is in .A

@L9:    ldx     #>$0000                 ; Clear high byte
        rts
.endproc
