;
; Ullrich von Bassewitz, 31.05.1998
;
; Several small file stream functions
;

        .export         _clearerr, _feof, _ferror, _fileno, _fflush
        .import         return0, ___directerrno
        .importzp       ptr1

        .include        "_file.inc"
        .include        "errno.inc"

;
; Get the FILE* parameter, check if the file is open. Returns zero in A
; and zero flag set in case of an error.

.proc   getf
        sta     ptr1
        stx     ptr1+1
        ldy     #_FILE::f_flags
        lda     (ptr1),y        ; get f->f_flags
        and     #_FOPEN         ; file open?
        rts
.endproc

;
; void clearerr (FILE* f);
;

.proc   _clearerr
        jsr     getf
        beq     err
        lda     (ptr1),y
        and     #<~(_FEOF | _FERROR)
        sta     (ptr1),y
err:    rts
.endproc

;
; int feof (FILE* f);
;

.proc   _feof
        jsr     getf
        beq     @L1             ; Return 0 on error
        lda     (ptr1),y
        and     #_FEOF
@L1:    ldx     #0
        rts
.endproc

;
; int ferror (FILE* f);
;

.proc   _ferror
        jsr     getf
        beq     @L1             ; Return 0 on error
        lda     (ptr1),y
        and     #_FERROR
@L1:    ldx     #0
        rts
.endproc

;
; int fileno (FILE* f);
;

.proc   _fileno
        jsr     getf
        beq     error
        ldy     #_FILE::f_fd
        lda     (ptr1),y
        ldx     #0
        rts

; If the file is not valid, fileno must set errno and return -1

error:  lda     #<EBADF
        jmp     ___directerrno
.endproc

;
; int __fastcall__ fflush (FILE* f);
;

_fflush = return0
