;
; Ullrich von Bassewitz, 17.11.2002
;
; Common stuff for the read/write routines
;

        .export         rwcommon

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp2

        .include        "filedes.inc"


;--------------------------------------------------------------------------
; rwcommon: Pop the parameters from stack, preprocess them and place them
; into zero page locations. Return carry set if the handle is invalid,
; return carry clear if it is ok. If the carry is clear, the handle is
; returned in A.

.proc   rwcommon

        jsr     popax           ; Get count
        eor     #$FF
        sta     ptr1
        txa
        eor     #$FF
        sta     ptr1+1          ; Remember -count-1

        jsr     popax           ; Get buf
        sta     ptr2
        stx     ptr2+1

        lda     #$00
        sta     ptr3
        sta     ptr3+1          ; Clear ptr3

        jsr     popax           ; Get the handle
        sta     tmp2
        cpx     #$01
        bcs     inv
        cmp     #MAX_FDS
inv:    rts

.endproc


