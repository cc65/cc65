;
; Colin Leroy-Mira, 2024
;
; Helper to check for file opened, not eof, not ferror
; Expects file pointer in ptr, returns 0 if everything is OK, -1 otherwise
; Destroys A, X, Y
;

        .export         checkferror
        .importzp       ptr1

        .include        "_file.inc"

checkferror:
        ldy     #_FILE::f_flags
        lda     (ptr1),y
        tax
        and     #_FOPEN         ; Check for file open
        beq     ret_eof
        txa
        and     #(_FERROR|_FEOF); Check for error/eof
        bne     ret_eof
        lda     #$00
        rts

ret_eof:
        lda     #$01
        rts
