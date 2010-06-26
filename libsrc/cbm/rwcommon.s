;
; Ullrich von Bassewitz, 17.11.2002
;
; Common stuff for the read/write routines
;

        .export         rwcommon

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp2

        .include        "errno.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; rwcommon: Pop the parameters from stack, preprocess them and place them
; into zero page locations. Return carry set if the handle is invalid,
; return carry clear if it is ok. If the carry is clear, the handle is
; returned in A.

.proc   rwcommon

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
        cpx     #$01
        bcs     @L9
        cmp     #MAX_FDS        ; Set carry if fd too large
        sta     tmp2
@L9:    rts                     ; Return with result in carry

.endproc


