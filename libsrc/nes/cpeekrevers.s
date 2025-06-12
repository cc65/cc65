;
; 2016-02-28, Groepaz
; 2017-08-17, Greg King
;
; char cpeekrevers (void);
;

        .export         _cpeekrevers

        .import         ppubuf_waitempty
        .forceimport    initconio

        .include        "nes.inc"


_cpeekrevers:
        ; wait until all console data has been written
        jsr     ppubuf_waitempty

        ldy     SCREEN_PTR+1
        lda     SCREEN_PTR

; waiting for vblank is incredibly slow ://
vwait:
;       ldx     PPU_STATUS
;       bpl     vwait

        ldx     #>$0000
        sty     PPU_VRAM_ADDR2
        sta     PPU_VRAM_ADDR2
        lda     PPU_VRAM_IO     ; first read is invalid
        lda     PPU_VRAM_IO     ; get data
        stx     PPU_VRAM_ADDR2
        stx     PPU_VRAM_ADDR2

        and     #<$80           ; get reverse bit
        rts
