;
; Ullrich von Bassewitz, 2004-05-12
;
; int __fastcall__ ungetc (int c, FILE* f);
; /* Push back a character into a file stream. */
;

        .export         _ungetc

        .import         popax
        .import         ptr1: zp, tmp1: zp

        .include        "_file.inc"
        .include        "errno.inc"


; ------------------------------------------------------------------------
; Code

.proc   _ungetc

; Save the file pointer to ptr1

        sta     ptr1
        stx     ptr1+1

; Get c from stack and save the lower byte in tmp1

        jsr     popax
        sta     tmp1

; c must be in char range

        txa
        bne     error

; Check if the file is open

        ldy     #_FILE::f_flags
        lda     (ptr1),y
        and     #_FOPEN                 ; Is the file open?
        beq     error                   ; Branch if no

; Set the pushback flag and reset the end-of-file indicator

        lda     (ptr1),y
        ora     #_FPUSHBACK
        and     #<~_FEOF
        sta     (ptr1),y

; Store the character into the pushback buffer

        ldy     #_FILE::f_pushback
        lda     tmp1
        sta     (ptr1),y

; Done, return c

        ldx     #0
        rts

; File is not open or the character is invalid

error:  lda     #EINVAL
        jmp     ___directerrno

.endproc
