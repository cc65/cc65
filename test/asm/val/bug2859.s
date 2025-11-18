; Tests for the bug reported in #2859. Boolean not had the correct precedence
; (as specified in the docs) but worked only correctly at the start of a full
; expression.

; This one assembles ok since ! is at the start
.if !.defined(__DOCART__) && .defined(__C64__)
.byte 1
.endif

; This one assembles too since a parenthesis restarts a full expression
.if .defined(__C64__) && (!.defined(__DOCART__))
.byte 2
.endif

; This one doesn't work since ! is somewhere in between
.if .defined(__C64__) && !.defined(__DOCART__)
.byte 3
.endif


        .import _exit
        .export _main

                 
; The following code is an indirect test for the precedence of .not.
; .not has the lowest precedence, so the expression that is loaded into A
; evaluates to zero. This has of course to be changed when the precedence
; of .not is changed.
_main:
        lda     #.not 0 + 1             ; Means: .not (0 + 1)
        ldx     #0
        jmp     _exit

