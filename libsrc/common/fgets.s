;
; Colin Leroy-Mira, 2024
;
; char* __fastcall__ fgets (char* s, unsigned size, register FILE* f)
;

        .export         _fgets
        .import         _fgetc, popptr1, pushptr1, popax, pushax, return0, ___errno
        .importzp       ptr1, ptr4

        .feature        string_escapes

        .include        "errno.inc"
        .include        "stdio.inc"
        .include        "_file.inc"

terminate_ptr:
        lda     #$00
        tax
        .if .cap(CPU_HAS_ZPIND)
        sta     (ptr4)
        .else
        tay
        sta     (ptr4),y
        .endif
        rts

_fgets:
        sta     ptr1
        stx     ptr1+1

        jsr     popax
        sta     size
        stx     size+1

        jsr     popax
        sta     ptr4
        stx     ptr4+1
        sta     buf
        stx     buf+1

        .if .cap(CPU_HAS_STZ)
        stz     didread
        .else
        lda     #$00            ; We have read nothing yet
        sta     didread
        .endif

        ; Check size
        lda     size
        ora     size+1
        bne     read_loop
        lda     #EINVAL
        sta     ___errno
        jmp     return0

read_loop:
        lda     size            ; Dec size
        bne     :+
        dec     size+1
:       dec     size

        bne     :+              ; Check bound
        ldx     size+1
        beq     done

:       jsr     pushptr1        ; Push ptr1 for backup and load it to AX for fgetc
        jsr     _fgetc          ; Read a char

        pha
        jsr     popptr1         ; Get ptr1 back
        pla

        cpx     #<EOF
        beq     got_eof

        ldy     #$01
        sty     didread         ; We read at least one char

        .if .cap(CPU_HAS_ZPIND)
        sta     (ptr4)
        .else
        dey
        sta     (ptr4),y
        .endif

        inc     ptr4
        bne     :+
        inc     ptr4+1

:       cmp     #'\n'    ; #'\n' should get translated properly
        beq     done
        bne     read_loop

got_eof:
        lda     didread
        beq     stopped_at_first_char
        ldy     #_FILE::f_flags
        lda     (ptr1),y
        and     #_FERROR
        bne     stopped_at_first_char

done:
        jsr     terminate_ptr
        ldx     #>buf
        lda     #<buf
        rts

stopped_at_first_char:
        jmp     terminate_ptr

        .bss

c:      .res 1
buf:    .res 2
size:   .res 2
didread:.res 1
