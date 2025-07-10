;
; Colin Leroy-Mira, 2024
;
; Helper to check for file opened, not eof, not ferror
; Expects file pointer in ptr1,
; Returns with Z flag set if everything is OK,
; Destroys A, X, Y,
; Sets file flags in A
;

        .export         checkferror
        .importzp       ptr1

        .include        "_file.inc"

checkferror:
        ldy     #_FILE::f_flags
        lda     (ptr1),y
        tax
        and     #(_FOPEN|_FERROR|_FEOF); Check for file open, error/eof
        tay
        txa
        cpy     #_FOPEN
        rts
