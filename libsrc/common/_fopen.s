;
; Ullrich von Bassewitz, 22.11.2002
;
; FILE* __fastcall__ _fopen (const char* name, const char* mode, FILE* f);
; /* Open the specified file and fill the descriptor values into f */
;

        .export         __fopen

        .import         _open
        .import         pushax, incsp4, return0
        .importzp       sp, ptr1


        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "_file.inc"


; ------------------------------------------------------------------------
; Code

.proc   __fopen

        sta     file
        stx     file+1          ; Save f

; Get a pointer to the mode string

        ldy     #1
        lda     (sp),y
        sta     ptr1+1
        dey
        lda     (sp),y
        sta     ptr1

; Look at the first character in mode

        ldx     #$00            ; Mode will be in X
        lda     (ptr1),y        ; Get first char from mode
        cmp     #'w'
        bne     @L1
        ldx     #(O_WRONLY | O_CREAT | O_TRUNC)
        bne     @L3
@L1:    cmp     #'r'
        bne     @L2
        ldx     #O_RDONLY
        bne     @L3
@L2:    cmp     #'a'
        bne     invmode
        ldx     #(O_WRONLY | O_CREAT | O_APPEND)

; Look at more chars from the mode string

@L3:    iny                     ; Next char
        beq     invmode
        lda     (ptr1),y
        beq     modeok          ; End of mode string reached
        cmp     #'+'
        bne     @L4
        txa
        ora     #O_RDWR         ; Always do r/w in addition to anything else
        tax
        bne     @L3
@L4:    cmp     #'b'
        beq     @L3             ; Binary mode is ignored

; Invalid mode

invmode:
        lda     #EINVAL
        jsr     __seterrno      ; Set __errno, returns zero in A
        tax                     ; a/x = 0
        jmp     incsp4

; Mode string successfully parsed. Store the binary mode onto the stack in
; the same place where the mode string pointer was before. Then call open()

modeok: ldy     #$00
        txa                     ; Mode -> A
        sta     (sp),y
        tya
        iny
        sta     (sp),y
        ldy     #4              ; Size of arguments in bytes
        jsr     _open           ; Will cleanup the stack

; Check the result of the open() call

        cpx     #$FF
        bne     openok
        cmp     #$FF
        bne     openok
        jmp     return0         ; Failure, errno/_oserror already set

; Open call succeeded

openok: ldy     file
        sty     ptr1
        ldy     file+1
        sty     ptr1+1
        ldy     #_FILE::f_fd
        sta     (ptr1),y        ; file->f_fd = fd;
        ldy     #_FILE::f_flags
        lda     #_FOPEN
        sta     (ptr1),y        ; file->f_flags = _FOPEN;

; Return the pointer to the file structure

        lda     ptr1
        ldx     ptr1+1
        rts

.endproc

; ------------------------------------------------------------------------
; Data

.bss
file:   .res    2


