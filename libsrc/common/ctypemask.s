; ctypemask.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; ctypemask(int c)
;
; converts a character to test via the is*-functions to the matching ctype-masks
; If c is out of the 8-bit range, the function returns with carry set and accu cleared.
; Return value is in accu and x has to be always clear when returning
; (makes calling code shorter)!
;
; IMPORTANT: stricmp, strlower, strnicmp, strupper and atoi rely that Y is not changed
; while calling this function!
;

        .export         ctypemask
        .export         ctypemaskdirect
        .import         __ctype
        .import         __ctypeidx

ctypemask:
        cpx     #$00            ; char range ok?
        bne     SC              ; branch if not
ctypemaskdirect:
        lsr     a
        tax
        lda     __ctypeidx,x
        bcc     @lowerNibble
@upperNibble:
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        clc                     ; remove out of bounds flag
@lowerNibble:
        and     #%00001111
        tax
        lda     __ctype,x
        ldx     #$00
        rts

SC:     sec
        lda     #$00
        tax
        rts
