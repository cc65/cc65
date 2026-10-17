;
; char* __fastcall__ getcwd (char* buf, size_t size);
;
; The common getcwd() copies __cwd, which nothing on this target writes, so
; this one reads the directory with f_getcwd().
;

        .export         _getcwd

        .import         __ria_getcwd
        .import         incsp2, popax, pushw0sp

        .include        "errno.inc"

_getcwd:
        pha
        phx
        jsr     pushw0sp        ; buf again, which __ria_getcwd removes
        plx
        pla
        ldy     #ERANGE
        jsr     __ria_getcwd
        inx                     ; X is $FF only on failure
        beq     @fail
        jmp     popax           ; buf
@fail:  txa
        jmp     incsp2          ; NULL
