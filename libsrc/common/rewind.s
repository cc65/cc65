;
; Colin Leroy-Mira <colin@colino.net>
;
; void __fastcall__ rewind (FILE* f)
; /* Rewind a file */
;

        .export         _rewind

        .import         _fseek, _clearerr
        .import         pushax, pushl0, popax

        .include        "stdio.inc"


; ------------------------------------------------------------------------
; Code

.proc   _rewind

        ; Push f twice (once for fseek, once for clearerr later)
        jsr     pushax
        jsr     pushax

        ; Push offset (long) zero
        jsr     pushl0

        lda     #SEEK_SET
        jsr     _fseek

        ; Clear error
        jsr     popax
        jmp     _clearerr

.endproc
