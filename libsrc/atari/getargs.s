; get arguments from command line (when DOS supports it)

; Freddy Offenga, 4/21/2000

; initmainargs is forcibly included by the C compiler if it encounters a
; main() function with arguments. Therefore it isn't referenced by the
; startup code but is nevertheless included in the compiled program when
; needed.

MAXARGS = 16            ; max. amount of arguments in arg. table
CL_SIZE = 64            ; command line buffer size
SPACE   = 32            ; SPACE char.

        .include        "atari.inc"
        .import         __argc, __argv
        .importzp       ptr1
        .import         __dos_type
        .constructor    initmainargs, 25

; --------------------------------------------------------------------------
; Get command line

.segment        "ONCE"

initmainargs:
        lda     #0
        sta     __argc
        sta     __argc+1
        sta     __argv
        sta     __argv+1

        lda     __dos_type      ; which DOS?
        cmp     #ATARIDOS
        beq     nargdos         ; DOS does not support arguments
        cmp     #MYDOS
        bne     argdos          ; DOS supports arguments
nargdos:rts

; Initialize ourcl buffer

argdos: lda     #ATEOL
        sta     ourcl+CL_SIZE

; Move SpartaDOS command line to our own buffer

        lda     DOSVEC
        clc
        adc     #<LBUF
        sta     ptr1
        lda     DOSVEC+1
        adc     #>LBUF
        sta     ptr1+1

        ldy     #0
cpcl:   lda     (ptr1),y
        sta     ourcl,y
        iny
        cmp     #ATEOL
        beq     movdon
        cpy     #CL_SIZE
        bne     cpcl

movdon: lda     #0
        sta     ourcl,y         ; null terminate behind ATEOL

; Turn command line into argv table

        ;ldy    #0
        tay
eatspc: lda     ourcl,y         ; eat spaces
        cmp     #ATEOL
        beq     finargs
        cmp     #SPACE
        bne     rpar            ; begin of argument found
        iny
        cpy     #CL_SIZE
        bne     eatspc
        beq     finargs         ; only spaces is no argument

; Store argument vector

rpar:   lda     __argc          ; low-byte
        asl
        tax                     ; table index
        tya                     ; ourcl index
        clc
        adc     #<ourcl
        sta     argv,x
        lda     #>ourcl
        adc     #0
        sta     argv+1,x
        ldx     __argc
        inx
        stx     __argc
        cpx     #MAXARGS
        beq     finargs

; Skip this arg.

skiparg:
        ldx     ourcl,y
        cpx     #ATEOL          ; end of line?
        beq     eopar
        cpx     #SPACE
        beq     eopar
        iny
        cpy     #CL_SIZE
        bne     skiparg

; End of arg. -> place 0

eopar:
        lda     #0
        sta     ourcl,y
        iny                     ; y behind arg.
        cpx     #ATEOL          ; was it the last arg?
        bne     eatspc

; Finish args

finargs:
        lda     __argc
        asl           
        tax
        lda     #0
        sta     argv,x
        sta     argv+1,x
        lda     #<argv
        ldx     #>argv
        sta     __argv
        stx     __argv+1
        rts

; --------------------------------------------------------------------------
; Data

.bss

argv:   .res    (1 + MAXARGS) * 2

; Buffer for command line / argv strings

ourcl:  .res    CL_SIZE+1
