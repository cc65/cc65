;
; Karri Kaksonen, 2010
;
; This function is used to place the Lynx hardware to point to any byte in
; the Lynx cart.
;
; This function supports all available block sizes (512, 1024 and 2048 bytes).
; No other block sizes have been used afaik.
;
; Only SEEK_SET operation mode is implemented.
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);

        .importzp       sp, sreg, regsave, regbank, tmp1, ptr1, ptr2
        .macpack        longbranch
        .export         _lseek
        .import         addysp, stax0sp, tosand0ax, pusheax, asreax2
        .import         ldeaxysp, decsp2, pushax, incsp8
        .import         tosandeax,decax1,tosdiveax,axlong,ldaxysp
        .import         lynxskip0, lynxblock,tosasreax
        .import         __BLOCKSIZE__
        .importzp       _FileCurrBlock

.segment        "CODE"

.proc   _lseek: near

.segment        "CODE"

        jsr     pushax
        ldy     #$05
        jsr     ldeaxysp
        jsr     pusheax
        ldx     #$00
        lda     #<(__BLOCKSIZE__/1024 + 9)
        jsr     tosasreax
        sta     _FileCurrBlock
        jsr     lynxblock
        ldy     #$05
        jsr     ldeaxysp
        jsr     pusheax
        lda     #<(__BLOCKSIZE__-1)
        ldx     #>(__BLOCKSIZE__-1)
        jsr     axlong
        jsr     tosandeax
        eor     #$FF
        pha
        txa
        eor     #$FF
        tay
        plx
        jsr     lynxskip0
        ldy     #$05
        jsr     ldeaxysp
        jmp     incsp8

.endproc

