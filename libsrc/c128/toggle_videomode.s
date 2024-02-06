;
; Ullrich von Bassewitz, 2003-02-13
;
; void toggle_videomode (void);
; /* Toggle the video mode between 40 and 80 chars (calls SWAPPER) */
;

        .export         _toggle_videomode
        .import         SWAPPER, BSOUT

; This function is deprecated
.assert         0, ldwarning, "toggle_videomode() is deprecated, please use videomode() instead!"



.proc   _toggle_videomode

        jsr     SWAPPER                 ; Toggle the mode
        lda     #14
        jmp     BSOUT                   ; Switch to lower case chars

.endproc

