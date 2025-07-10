;
; Colin Leroy-Mira, 2024
;
; int __fastcall__ fgetc (register FILE* f)
;

        .export         _fgetc
        .import         _read, checkferror
        .import         pusha0, pushax, popptr1, incsp2, returnFFFF
        .importzp       ptr1

        .include        "stdio.inc"
        .include        "_file.inc"

_fgetc:
        sta     ptr1
        stx     ptr1+1
        jsr     pushax          ; Backup our ptr

        jsr     checkferror
        bne     ret_eof

        .if .cap(CPU_HAS_BITIMM)
        bit     #_FPUSHBACK     ; Check for pushed back char
        beq     do_read
        .else
        tax
        and     #_FPUSHBACK     ; Check for pushed back char
        beq     do_read
        txa
        .endif

        and     #<(~_FPUSHBACK) ; Reset flag
        sta     (ptr1),y

        .assert _FILE::f_pushback = _FILE::f_flags+1, error
        iny
        jsr     incsp2          ; Drop our ptr copy
        lda     (ptr1),y        ; Return pushed back char
        ldx     #$00
        rts

do_read:
        ; Push _read parameters
        ldy     #_FILE::f_fd
        lda     (ptr1),y
        jsr     pusha0

        lda     #<c
        ldx     #>c
        jsr     pushax

        lda     #$01
        ldx     #$00

        ; Read
        jsr     _read

        ; Check for errors
        cmp     #$00
        beq     set_feof

        cmp     #<(-1)
        beq     set_ferror

        jsr     incsp2
        ; Return char
        ldx     #$00
        lda     c
        rts

ret_eof:
        jsr     incsp2
        jmp     returnFFFF

set_ferror:
        lda     #_FERROR
        bne     set_err
set_feof:
        lda     #_FEOF
set_err:
        pha
        jsr     popptr1
        pla
        ldy     #_FILE::f_flags
        ora     (ptr1),y
        sta     (ptr1),y
        jmp     returnFFFF

        .bss

c:      .res 1
