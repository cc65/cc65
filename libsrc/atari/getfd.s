;
; Christian Groessler, Oct-2000
;
; allocates a new fd in the indirection table
;

        .include "atari.inc"
        .include "fd.inc"
        .include "_file.inc"
        .importzp tmp1
        .import fd_table, fd_index

        .export fdt_to_fdi,getfd

        .code

; fdt_to_fdi
; returns a fd_index entry pointing to the given ft_table entry
; get fd_table entry in A
; return C = 0/1 for OK/error
; return fd_index entry in A if OK
; registers destroyed
.proc   fdt_to_fdi

        tay
        lda     #$ff
        tax
        inx
loop:   cmp     fd_index,x
        beq     found
        inx
        cpx     #MAX_FD_INDEX
        bcc     loop
        rts

found:  tya
        sta     fd_index,x
        txa
        clc
        rts

.endproc

; getfd
; get a new fd pointing to a ft_table entry
; usage counter of ft_table entry incremented
; A - fd_table entry
; return C = 0/1 for OK/error
; returns fd in A if OK
; registers destroyed, tmp1 destroyed
.proc   getfd

        sta     tmp1            ; save fd_table entry
        jsr     fdt_to_fdi
        bcs     error

        pha
        lda     tmp1
        asl     a
        asl     a                       ; also clears C
        tax
        inc     fd_table+ft_usa,x       ; increment usage counter
        pla
error:  rts

.endproc
