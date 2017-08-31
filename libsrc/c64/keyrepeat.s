;
; Normally, the keyboards on some Commodore models repeat only their "editor"
; keys (cursor movers, delete, insert, and space).  The keyboards on other CBM
; models, and many other platforms, repeat all keys, normally.  This constructor
; makes Commodore keyboards repeat, so that portable programs can assume that
; all platforms with keyboards repeat all keys.
;
; This module is linked by the conio and POSIX input functions.
;
; 2017-08-20, Greg King
;

        .constructor    keys_repeat
        .destructor     keys_normal

        .include        "c64.inc"


;--------------------------------------------------------------------------
; Put this constructor into a segment whose space
; will be re-used by BSS, the heap, and the C stack.
;
.segment        "ONCE"

; Make all keys repeat.

keys_repeat:
        lda     KBDREPEAT
        sta     rptsave
        lda     #%10000000
        sta     KBDREPEAT
        rts


;--------------------------------------------------------------------------

.code

; Restore the old keyboard-repeat state.

keys_normal:
        lda     rptsave
        sta     KBDREPEAT
        rts


;--------------------------------------------------------------------------

.segment        "INIT"

rptsave:
        .res    1
