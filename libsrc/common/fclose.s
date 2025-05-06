;
; Ullrich von Bassewitz, 22.11.2002
;
; int __fastcall__ fclose (FILE* f);
; /* Close a file */
;

        .export         _fclose

        .import         _close, ___directerrno
        .importzp       ptr1

        .include        "errno.inc"
        .include        "_file.inc"

; ------------------------------------------------------------------------
; Code

.proc   _fclose

        sta     ptr1
        stx     ptr1+1          ; Store f

; Check if the file is really open

        ldy     #_FILE::f_flags
        lda     (ptr1),y
        and     #_FOPEN
        bne     @L1

; File is not open

        lda     #EINVAL
        jmp     ___directerrno

; File is open. Reset the flags and close the file.

@L1:    lda     #_FCLOSED
        sta     (ptr1),y

        ldy     #_FILE::f_fd
        lda     (ptr1),y
        ldx     #0
        jmp     _close          ; Will set errno and return an error flag

.endproc
