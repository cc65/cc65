;
; Groepaz/Hitmen, 19.10.2015
;
; high level implementation for the monochrome soft80 implementation
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
;

        .export         soft80mono_textcolor, soft80mono_bgcolor
        .import         soft80mono_internal_cellcolor, soft80mono_internal_bgcolor

        .importzp       tmp1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80mono_textcolor:
        ldx     CHARCOLOR                       ; get old value
        stx     tmp1                            ; save old value
        sta     CHARCOLOR                       ; set new value

mkcharcolor:
        lda     soft80mono_internal_bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        ora     CHARCOLOR
        sta     soft80mono_internal_cellcolor   ; text/bg combo for new chars

        sei
        ldy     $01
        lda     #$34                            ; enable RAM under I/O
        sta     $01

        lda     soft80mono_internal_cellcolor
        ; clear loop for vram
        ldx     #$00
@lp1:
        sta     soft80_vram,x
        sta     soft80_vram+$100,x
        sta     soft80_vram+$200,x
        sta     soft80_vram+$2e8,x
        inx
        bne     @lp1

        sty     $01
        cli

        lda     tmp1                            ; get old value
        rts

soft80mono_bgcolor:
        ldx     soft80mono_internal_bgcolor     ; get old value
        stx     tmp1                            ; save old value
        sta     soft80mono_internal_bgcolor     ; set new value

        jmp     mkcharcolor

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80mono_init
conio_init      = soft80mono_init
