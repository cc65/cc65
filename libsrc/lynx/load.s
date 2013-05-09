;
; Karri Kaksonen, 2010
;
; lynx_load(fileno) is a convenience function that is widely used on the Lynx.
; Basically this opens directory entry fileno and reads the content of the
; file this points to into RAM.
;
; void lynx_load(int fileno)
;
        .importzp       _FileFileLen
        .importzp       _FileDestAddr
        .import         pushax,ldax0sp,pusha0,incsp2
        .import         _openn
        .import         _read
        .export         _lynx_load

; ---------------------------------------------------------------
; void __near__ __fastcall__ lynx_load (int)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _lynx_load: near

.segment        "CODE"

        jsr     pushax
        jsr     ldax0sp
        jsr     _openn
        lda     #$01
        jsr     pusha0
        lda     _FileDestAddr
        ldx     _FileDestAddr+1
        jsr     pushax
        lda     _FileFileLen
        ldx     _FileFileLen+1
        jsr     _read
        jmp     incsp2

.endproc

