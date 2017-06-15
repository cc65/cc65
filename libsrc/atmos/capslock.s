;
; When Oric computers are in BASIC's command mode, the keyboard is in CAPS lock
; mode (because Oric BASIC keywords must be typed in upper-case).  This
; constructor disables that mode, so that text will be typed as lower-case
; (which is the default on other cc65 platforms).
; This module is linked by the conio and POSIX input functions.
;
; 2014-09-04, Greg King
;

        .constructor    disable_caps
        .destructor     restore_caps

        .include        "atmos.inc"


;--------------------------------------------------------------------------
; Put this constructor into a segment whose space
; will be re-used by BSS, the heap, and the C stack.
;
.segment        "ONCE"

; Turn the capitals lock off.

disable_caps:
        lda     CAPSLOCK
        sta     capsave
        lda     #$7F
        sta     CAPSLOCK
        rts


;--------------------------------------------------------------------------

.code

; Restore the old capitals-lock state.

restore_caps:
        lda     capsave
        sta     CAPSLOCK
        rts


;--------------------------------------------------------------------------

.segment        "INIT"

capsave:
        .res    1
