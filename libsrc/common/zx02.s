; void __fastcall__ decompress_zx02(const void *src, void *dest)
;
; De-compressor for ZX02 files
;
; Compress with:
;    zx02 input.bin output.zx0
;
; (c) 2022 DMSC
; Code under MIT license, see LICENSE file.

        .export         _decompress_zx02

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

offset_hi = tmp1
ZX0_src   = ptr1
ZX0_dst   = ptr2
bitr      = tmp2
pntr      = ptr3

.proc _decompress_zx02
        sta     ZX0_dst
        stx     ZX0_dst+1

        jsr     popax
        sta     ZX0_src
        stx     ZX0_src+1

        ; Init values
        lda     #$80
        sta     bitr
        ldy     #$FF
        sty     pntr
        iny
        sty     offset_hi ; Y = 0 at end of init

; Decode literal: Copy next N bytes from compressed file
; Elias(length)  byte[1]  byte[2]  ...  byte[N]
decode_literal:
        ldx     #$01
        jsr     get_elias

cop0:
        lda     (ZX0_src), y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

:       sta     (ZX0_dst),y
        inc     ZX0_dst
        bne     :+
        inc     ZX0_dst+1

:       dex
        bne     cop0

        asl     bitr
        bcs     dzx0s_new_offset

        ; Copy from last offset (repeat N bytes from last offset)
        ; Elias(length)
        inx
        jsr     get_elias

dzx0s_copy:
        lda     ZX0_dst+1
        sbc     offset_hi  ; C=0 from get_elias
        sta     pntr+1

cop1:
        ldy     ZX0_dst
        lda     (pntr), y
        ldy     #0
        sta     (ZX0_dst),y
        inc     ZX0_dst
        bne     :+
        inc     ZX0_dst+1
        inc     pntr+1
:       dex
        bne     cop1

        asl     bitr
        bcc     decode_literal

; Copy from new offset (repeat N bytes from new offset)
; Elias(MSB(offset))  LSB(offset)  Elias(length-1)
dzx0s_new_offset:
        ; Read elias code for high part of offset
        inx
        jsr     get_elias
        beq     exit  ; Read a 0, signals the end

        ; Decrease and divide by 2
        dex
        txa
        lsr
        sta     offset_hi

        ; Get low part of offset, a literal 7 bits
        lda     (ZX0_src), y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

:       ; Divide by 2
        ror
        eor     #$ff
        sta     pntr

        ; And get the copy length.
        ; Start elias reading with the bit already in carry:
        ldx     #1
        jsr     elias_skip1

        inx
        bcc     dzx0s_copy

; Read an elias-gamma interlaced code.
elias_get:
        ; Read next data bit to result
        asl     bitr
        rol
        tax

get_elias:
        ; Get one bit
        asl     bitr
        bne     elias_skip1

        ; Read new bit from stream
        lda     (ZX0_src), y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

:     ; sec   ; not needed, C=1 guaranteed from last bit
        rol
        sta     bitr

elias_skip1:
        txa
        bcs     elias_get

        ; Got ending bit, stop reading
exit:
        rts
.endproc
