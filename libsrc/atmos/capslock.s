;
; When Oric computers are in BASIC's command mode, the keyboard is in CAPS lock
; mode (because Oric BASIC keywords must be typed in upper-case).  This
; constructor disables that mode, so that text will be typed as lower-case
; (which is the default on other cc65 platforms).
; This module is linked by the conio and POSIX input functions.
;
; 2014-08-22, Greg King
;

        .destructor     restore_caps
        .constructor    disable_caps

        .include        "atmos.inc"


;--------------------------------------------------------------------------

; Restore the old capitals-lock state.

restore_caps:
        lda     capsave
        sta     CAPSLOCK
        rts


;--------------------------------------------------------------------------
; Put this constructor into a segment that can be re-used by programs.
;
.segment        "INIT"

; Turn the capitals lock off.

disable_caps:
        lda     CAPSLOCK
        sta     capsave
        lda     #$7F
        sta     CAPSLOCK
        rts


;--------------------------------------------------------------------------

.bss

capsave:
        .res    1
