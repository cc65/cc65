;
; Ullrich von Bassewitz, 2003-06-12
;
; int __fastcall__ creat (const char* name, unsigned mode);
;

        .export         _creat
        .import         _open
        .import         pushax

        .include        "fcntl.inc"


; The call
;
;       creat (name, mode);
;
; is equivalent to
;
;       open (name, O_CREAT | O_WRONLY | O_TRUNC, mode);
;


.proc   _creat

        pha
        txa
        pha                             ; Save mode

        lda     #<(O_CREAT | O_WRONLY | O_TRUNC)
        ldx     #>(O_CREAT | O_WRONLY | O_TRUNC)
        jsr     pushax

        pla
        tax
        pla
        jsr     pushax                  ; Push mode on argument stack

        ldy     #6                      ; Number of argument bytes
        jmp     _open

.endproc



