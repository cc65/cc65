;
; Ullrich von Bassewitz, 2003-03-07
;
; Setup arguments for main.
; Based on code from Stefan A. Haubenthal, <polluks@web.de>
;


	.constructor    initmainargs, 24
       	.import         __argc, __argv

        .include        "c64.inc"

;---------------------------------------------------------------------------
; Setup arguments for main

.proc   initmainargs

; Setup a pointer to our argv vector

        lda     #<argv
        sta     __argv
        lda     #>argv
        sta     __argv+1

; Save the last filename as argument #0. Since the buffer we're copying into
; is zeroed out, we don't need to add a NUL character.

        ldy     FNAM_LEN
        cpy     #16+1
        bcc     L0
        ldy     #16             ; Limit the length
L0:     dey
L1:     lda     (FNAM),y
        sta     argv0,y
        dey
        bpl     L1
        inc     __argc          ; __argc = 1

; Find argument in BASIC buffer, if found, use it as arg #1

        ldy     #0
L2:     lda     $200,y
        beq     L9
        iny
        cmp     #$8F            ; REM token
        bne     L2
        sty     argv+2          ; Store offset
        ldy     #>$200
        sty     argv+3
        inc     __argc          ; argc = 2

; Done

L9:     rts

.endproc


;---------------------------------------------------------------------------
; Data

.data

argv:   .word   argv0           ; Pointer to program name
        .word   $0000           ; Optional second argument
        .word   $0000           ; Last vector must always be NULL

.bss
argv0:  .res    17              ; Program name


